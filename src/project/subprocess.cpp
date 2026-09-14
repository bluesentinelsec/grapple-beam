#include "project.hpp"

#include <SDL3/SDL.h>
#include <cerrno>
#include <cstring>
#include <stdexcept>
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#else
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace grapple::project
{
namespace
{
constexpr size_t kOutputLimit = 16 * 1024 * 1024;
#ifdef _WIN32
std::wstring Wide(const std::string &text)
{
    const int size =
        MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.c_str(), -1, nullptr, 0);
    if (!size)
        throw std::runtime_error("Command contains invalid UTF-8");
    std::wstring result(static_cast<size_t>(size), L'\0');
    MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.c_str(), -1, result.data(), size);
    result.pop_back();
    return result;
}
std::wstring Quote(const std::string &arg)
{
    std::wstring result = L"\"";
    size_t slashes = 0;
    for (wchar_t ch : Wide(arg))
    {
        if (ch == L'\\')
        {
            ++slashes;
            continue;
        }
        result.append(ch == L'"' ? slashes * 2 + 1 : slashes, L'\\');
        slashes = 0;
        result += ch;
    }
    result.append(slashes * 2, L'\\');
    return result + L'"';
}
struct Handle
{
    HANDLE value = nullptr;
    ~Handle()
    {
        if (value && value != INVALID_HANDLE_VALUE)
            CloseHandle(value);
    }
};
#else
struct Pipe
{
    int ends[2]{-1, -1};
    ~Pipe()
    {
        for (int fd : ends)
            if (fd >= 0)
                close(fd);
    }
};
#endif
} // namespace
ProcessResult Run(const Command &args, const std::filesystem::path &cwd, int timeout_seconds)
{
    if (args.empty() || timeout_seconds <= 0)
        throw std::invalid_argument("A command and positive timeout are required");
    ProcessResult result;
    const auto start = SDL_GetTicks();
#ifdef _WIN32
    Handle input, output, job, thread, process, null_input;
    SECURITY_ATTRIBUTES security{sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE};
    if (!CreatePipe(&input.value, &output.value, &security, 0) ||
        !SetHandleInformation(input.value, HANDLE_FLAG_INHERIT, 0))
        throw std::runtime_error("Cannot create command output pipe");
    job.value = CreateJobObjectW(nullptr, nullptr);
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION limit{};
    limit.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    if (!job.value || !SetInformationJobObject(job.value, JobObjectExtendedLimitInformation, &limit,
                                               sizeof(limit)))
        throw std::runtime_error("Cannot create bounded process job");
    null_input.value = CreateFileW(L"NUL", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                   &security, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (null_input.value == INVALID_HANDLE_VALUE)
        throw std::runtime_error("Cannot open process input");
    STARTUPINFOW startup{};
    startup.cb = sizeof(startup);
    startup.dwFlags = STARTF_USESTDHANDLES;
    startup.hStdInput = null_input.value;
    startup.hStdOutput = startup.hStdError = output.value;
    PROCESS_INFORMATION info{};
    std::wstring line;
    for (const auto &arg : args)
    {
        if (!line.empty())
            line += L' ';
        line += Quote(arg);
    }
    const auto directory = cwd.empty() ? std::wstring{} : cwd.wstring();
    if (!CreateProcessW(nullptr, line.data(), nullptr, nullptr, TRUE,
                        CREATE_SUSPENDED | CREATE_NO_WINDOW, nullptr,
                        directory.empty() ? nullptr : directory.c_str(), &startup, &info))
        throw std::runtime_error("Cannot start " + args.front() + "; Windows error " +
                                 std::to_string(GetLastError()));
    process.value = info.hProcess;
    thread.value = info.hThread;
    if (!AssignProcessToJobObject(job.value, process.value))
    {
        TerminateProcess(process.value, 1);
        WaitForSingleObject(process.value, INFINITE);
        throw std::runtime_error("Cannot assign child to a bounded process job");
    }
    CloseHandle(output.value);
    output.value = nullptr;
    if (ResumeThread(thread.value) == static_cast<DWORD>(-1))
        throw std::runtime_error("Cannot resume child process");
    auto drain = [&] {
        DWORD available = 0;
        while (PeekNamedPipe(input.value, nullptr, 0, nullptr, &available, nullptr) && available)
        {
            char buffer[8192];
            DWORD count = 0;
            if (!ReadFile(input.value, buffer, static_cast<DWORD>(sizeof(buffer)), &count, nullptr))
                break;
            result.output.append(buffer, count);
            if (result.output.size() > kOutputLimit)
                throw std::runtime_error("Command exceeded the output limit");
        }
    };
    while (WaitForSingleObject(process.value, 0) == WAIT_TIMEOUT)
    {
        drain();
        if (SDL_GetTicks() - start > static_cast<Uint64>(timeout_seconds) * 1000)
        {
            TerminateJobObject(job.value, 1);
            WaitForSingleObject(process.value, INFINITE);
            throw std::runtime_error(args.front() + " timed out");
        }
        SDL_Delay(10);
    }
    drain();
    DWORD status = 1;
    if (!GetExitCodeProcess(process.value, &status))
        throw std::runtime_error("Cannot read process exit status");
    result.status = status > 255 ? 1 : static_cast<int>(status);
#else
    Pipe pipe;
    if (::pipe(pipe.ends) != 0)
        throw std::runtime_error("Cannot create subprocess pipe");
    std::vector<char *> pointers;
    for (const auto &arg : args)
        pointers.push_back(const_cast<char *>(arg.c_str()));
    pointers.push_back(nullptr);
    const auto directory = cwd.native();
    const pid_t pid = fork();
    if (pid < 0)
        throw std::runtime_error("Cannot fork " + args.front());
    if (pid == 0)
    {
        // Only async-signal-safe operations are allowed between fork and exec.
        bool ok = setpgid(0, 0) == 0;
        const int input = open("/dev/null", O_RDONLY);
        ok = ok && input >= 0 && dup2(input, STDIN_FILENO) >= 0 &&
             dup2(pipe.ends[1], STDOUT_FILENO) >= 0 && dup2(pipe.ends[1], STDERR_FILENO) >= 0;
        if (input > STDERR_FILENO)
            close(input);
        close(pipe.ends[0]);
        close(pipe.ends[1]);
        if (!directory.empty())
            ok = ok && chdir(directory.c_str()) == 0;
        if (ok)
            execvp(pointers[0], pointers.data());
        const char error[] =
            "Cannot start subprocess; check executable availability and working directory\n";
        const auto ignored = write(STDERR_FILENO, error, sizeof(error) - 1);
        (void)ignored;
        _exit(127);
    }
    struct Child
    {
        pid_t pid;
        bool reaped = false;
        ~Child()
        {
            if (!reaped)
            {
                kill(-pid, SIGKILL);
                while (waitpid(pid, nullptr, 0) < 0 && errno == EINTR)
                {
                }
            }
        }
    } child{pid};
    close(pipe.ends[1]);
    pipe.ends[1] = -1;
    if (fcntl(pipe.ends[0], F_SETFL, O_NONBLOCK) < 0)
        throw std::runtime_error("Cannot make command output nonblocking");
    auto drain = [&] {
        char buffer[8192];
        while (true)
        {
            const auto count = read(pipe.ends[0], buffer, sizeof(buffer));
            if (count > 0)
            {
                result.output.append(buffer, static_cast<size_t>(count));
                if (result.output.size() > kOutputLimit)
                    throw std::runtime_error("Command exceeded the output limit");
            }
            else if (count < 0 && errno == EINTR)
                continue;
            else if (count < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
                throw std::runtime_error("Cannot read command output");
            else
                break;
        }
    };
    int status = 0;
    while (true)
    {
        drain();
        const auto waited = waitpid(pid, &status, WNOHANG);
        if (waited == pid)
        {
            child.reaped = true;
            break;
        }
        if (waited < 0 && errno != EINTR)
            throw std::runtime_error("Cannot wait for child process");
        if (SDL_GetTicks() - start > static_cast<Uint64>(timeout_seconds) * 1000)
            throw std::runtime_error(args.front() + " timed out");
        SDL_Delay(10);
    }
    drain();
    result.status = WIFEXITED(status) ? WEXITSTATUS(status) : 1;
#endif
    return result;
}
} // namespace grapple::project
