/**
 * @file pong.cpp
 * @brief Pong, in C++, on the opinionated loop.
 *
 * Original grapple-beam code (zlib). Deliberately the same game as pong.c,
 * structured the same way, so the two can be read side by side to see what
 * the C++ surface changes and what it does not.
 *
 * What it changes: the engine is an owning object, so there is no destroy
 * call and no leak on an early return; hooks are lambdas that capture the
 * game, so there is no void* to cast back; errors arrive as Result rather
 * than a null and SDL_GetError().
 *
 * What it does not change: the loop. fixed_update still runs at a fixed rate
 * and render still interpolates by alpha, because that is the engine's shape
 * rather than the language's.
 */
#include <grapple/grapple.h>

#include <grapple/engine.h>
#include <grapple/engine_input.h>
#include <grapple/debug_text.h>

#include <SDL3/SDL.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <random>

namespace {

constexpr int kFieldWidth = 1280;
constexpr int kFieldHeight = 720;

constexpr float kPaddleWidth = 18.0f;
constexpr float kPaddleHeight = 120.0f;
constexpr float kPaddleInset = 48.0f;
constexpr float kPaddleSpeed = 620.0f;

constexpr float kBallSize = 18.0f;
constexpr float kBallSpeed = 520.0f;
constexpr float kBallSpeedup = 1.04f;
constexpr float kBallMaxSpeed = 1500.0f;

constexpr int kWinningScore = 11;

/// Where to draw something that moved between the last two simulation steps.
float Interpolate(float previous, float current, float alpha) {
  return previous + (current - previous) * alpha;
}

/// A moving thing, and where it was one step ago. `previous` exists only so
/// rendering can interpolate; drawing the live position stutters whenever the
/// display and the simulation disagree about when a frame happens.
struct Mover {
  float x = 0.0f;
  float y = 0.0f;
  float previous_x = 0.0f;
  float previous_y = 0.0f;
  float velocity_x = 0.0f;
  float velocity_y = 0.0f;

  SDL_FRect Rect(float width, float height, float alpha) const {
    return SDL_FRect{Interpolate(previous_x, x, alpha),
                     Interpolate(previous_y, y, alpha), width, height};
  }
};

class Pong {
 public:
  Pong(grapple::Engine& engine, bool narrate)
      : engine_(engine), narrate_(narrate) {
    ResetMatch();
  }

  void FixedUpdate(float step) {
    MovePaddle(left_, PaddleInput(SDL_SCANCODE_W, SDL_SCANCODE_S, 0), step);
    MovePaddle(right_, PaddleInput(SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, 1), step);

    if (serve_delay_ > 0.0f) {
      serve_delay_ -= step;
      return;
    }

    ball_.previous_x = ball_.x;
    ball_.previous_y = ball_.y;
    ball_.x += ball_.velocity_x * step;
    ball_.y += ball_.velocity_y * step;

    BounceOffWalls();
    BounceOffPaddles();
    AwardPoints();

    if (Grapple_KeyPressed(engine_.get(), SDL_SCANCODE_ESCAPE)) {
      engine_.Quit();
    }
  }

  void Render(float alpha) const {
    SDL_Renderer* renderer = engine_.renderer();

    SDL_SetRenderDrawColor(renderer, 16, 16, 22, 255);
    SDL_RenderClear(renderer);
    DrawNet(renderer);

    SDL_SetRenderDrawColor(renderer, 235, 235, 240, 255);
    const SDL_FRect left = left_.Rect(kPaddleWidth, kPaddleHeight, alpha);
    const SDL_FRect right = right_.Rect(kPaddleWidth, kPaddleHeight, alpha);
    const SDL_FRect ball = ball_.Rect(kBallSize, kBallSize, alpha);
    SDL_RenderFillRect(renderer, &left);
    SDL_RenderFillRect(renderer, &right);
    SDL_RenderFillRect(renderer, &ball);

    Grapple_SetDebugTextSize(48.0f);
    Grapple_RenderDebugTextFormat(renderer, kFieldWidth * 0.5f - 120.0f, 32.0f, "%d",
                                  left_score_);
    Grapple_RenderDebugTextFormat(renderer, kFieldWidth * 0.5f + 80.0f, 32.0f, "%d",
                                  right_score_);
    Grapple_SetDebugTextSize(18.0f);
    Grapple_RenderDebugText(renderer, 24.0f, kFieldHeight - 40.0f,
                            "W/S and Up/Down — first to 11 — Esc quits");
  }

 private:
  void ResetMatch() {
    left_.x = kPaddleInset;
    right_.x = kFieldWidth - kPaddleInset - kPaddleWidth;
    left_.y = right_.y = kFieldHeight * 0.5f - kPaddleHeight * 0.5f;
    left_.previous_y = left_.y;
    right_.previous_y = right_.y;
    left_score_ = right_score_ = 0;
    serve_direction_ = 1.0f;
    PlaceBallForServe();
  }

  void PlaceBallForServe() {
    ball_.x = kFieldWidth * 0.5f - kBallSize * 0.5f;
    ball_.y = kFieldHeight * 0.5f - kBallSize * 0.5f;
    ball_.previous_x = ball_.x;
    ball_.previous_y = ball_.y;

    // Every serve leans up or down, and never by so little that it is a flat
    // line both paddles can meet without moving.
    std::uniform_real_distribution<float> steepness(0.4f, 1.0f);
    std::bernoulli_distribution upward(0.5);
    const float lean = upward(random_) ? 1.0f : -1.0f;
    ball_.velocity_x = kBallSpeed * serve_direction_;
    ball_.velocity_y = kBallSpeed * 0.35f * lean * steepness(random_);
    serve_delay_ = 0.75f;
  }

  /// One paddle's worth of input: keyboard, or a gamepad stick if one is
  /// plugged in and the keys are idle.
  float PaddleInput(SDL_Scancode up, SDL_Scancode down, int gamepad_player) const {
    float direction = 0.0f;
    if (Grapple_KeyDown(engine_.get(), up)) direction -= 1.0f;
    if (Grapple_KeyDown(engine_.get(), down)) direction += 1.0f;

    if (direction == 0.0f && Grapple_GamepadConnected(engine_.get(), gamepad_player)) {
      float stick_x = 0.0f;
      float stick_y = 0.0f;
      Grapple_GamepadStick(engine_.get(), gamepad_player, 0, &stick_x, &stick_y);
      direction = stick_y;
    }
    return direction;
  }

  static void MovePaddle(Mover& paddle, float direction, float step) {
    paddle.previous_y = paddle.y;
    paddle.y = std::clamp(paddle.y + direction * kPaddleSpeed * step, 0.0f,
                          static_cast<float>(kFieldHeight) - kPaddleHeight);
  }

  void BounceOffWalls() {
    if (ball_.y <= 0.0f) {
      ball_.y = 0.0f;
      ball_.velocity_y = -ball_.velocity_y;
    } else if (ball_.y + kBallSize >= kFieldHeight) {
      ball_.y = kFieldHeight - kBallSize;
      ball_.velocity_y = -ball_.velocity_y;
    }
  }

  /// Send the ball back, steeper the further from the paddle's middle it hit.
  /// That single rule is what turns Pong from a demo into a game: it gives the
  /// player a way to aim.
  void Deflect(const Mover& paddle, float direction) {
    const float paddle_middle = paddle.y + kPaddleHeight * 0.5f;
    const float ball_middle = ball_.y + kBallSize * 0.5f;
    const float offset = (ball_middle - paddle_middle) / (kPaddleHeight * 0.5f);

    const float speed = std::min(
        kBallMaxSpeed, std::hypot(ball_.velocity_x, ball_.velocity_y) * kBallSpeedup);
    ball_.velocity_x = speed * direction;
    ball_.velocity_y = speed * std::clamp(offset, -1.0f, 1.0f);

    // Nudge clear, or a fast ball registers a second hit next step and sticks.
    ball_.x = (direction > 0.0f) ? paddle.x + kPaddleWidth : paddle.x - kBallSize;
  }

  void BounceOffPaddles() {
    const SDL_FRect ball = ball_.Rect(kBallSize, kBallSize, 1.0f);
    const SDL_FRect left = left_.Rect(kPaddleWidth, kPaddleHeight, 1.0f);
    const SDL_FRect right = right_.Rect(kPaddleWidth, kPaddleHeight, 1.0f);

    if (ball_.velocity_x < 0.0f && SDL_HasRectIntersectionFloat(&ball, &left)) {
      Deflect(left_, 1.0f);
    } else if (ball_.velocity_x > 0.0f && SDL_HasRectIntersectionFloat(&ball, &right)) {
      Deflect(right_, -1.0f);
    }
  }

  void AwardPoints() {
    if (ball_.x + kBallSize < 0.0f) {
      ++right_score_;
      Narrate();
      serve_direction_ = 1.0f;
      PlaceBallForServe();
    } else if (ball_.x > kFieldWidth) {
      ++left_score_;
      Narrate();
      serve_direction_ = -1.0f;
      PlaceBallForServe();
    }
    if (left_score_ >= kWinningScore || right_score_ >= kWinningScore) {
      ResetMatch();
    }
  }

  static void DrawNet(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 60, 60, 70, 255);
    for (float y = 8.0f; y < kFieldHeight; y += 32.0f) {
      const SDL_FRect dash{kFieldWidth * 0.5f - 2.0f, y, 4.0f, 16.0f};
      SDL_RenderFillRect(renderer, &dash);
    }
  }

  // With no window there is nothing to watch, so the match is narrated to the
  // log: that is how CI tells a played game from a frozen one.
  void Narrate() const {
    if (narrate_) {
      SDL_Log("score %d-%d", left_score_, right_score_);
    }
  }

  grapple::Engine& engine_;
  Mover left_;
  Mover right_;
  Mover ball_;
  bool narrate_ = false;
  int left_score_ = 0;
  int right_score_ = 0;
  float serve_delay_ = 0.0f;
  float serve_direction_ = 1.0f;
  std::mt19937 random_{std::random_device{}()};
};

}  // namespace

int main() {
  Grapple_EngineConfig config{};
  config.title = "Pong — grapple-beam";
  config.design_width = kFieldWidth;
  config.design_height = kFieldHeight;

  // Pong is drawn from rectangles: there is no media archive to mount.
  config.no_auto_mount = true;
  // Headless draws to a surface rather than a window, which is how CI plays
  // this on a machine with no display.
  config.headless = SDL_getenv("GRAPPLE_HEADLESS") != nullptr;

  grapple::Result<grapple::Engine> engine = grapple::Engine::Create(config);
  if (!engine.ok()) {
    std::fprintf(stderr, "could not create an engine: %s\n",
                 engine.status().message().c_str());
    return 1;
  }

  Pong pong(*engine, config.headless);
  engine->on_fixed_update([&pong](float step) { pong.FixedUpdate(step); });
  engine->on_render([&pong](float alpha) { pong.Render(alpha); });

  return engine->Run() ? 0 : 1;
}
