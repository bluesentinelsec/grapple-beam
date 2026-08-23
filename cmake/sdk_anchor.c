/*
 * sdk_anchor.c — one real source file for the SDK archive.
 * Original Grapple code (zlib).
 *
 * The SDK is assembled entirely from $<TARGET_OBJECTS:...>. Under Ninja and
 * Make that is enough; the Xcode generator treats a static library with no
 * compiled sources of its own as having nothing to do, reports success, and
 * writes no archive at all — which is a worse failure than an error, because
 * it is green.
 *
 * So the target gets one source. It exists to be compiled.
 */
const char *Grapple_SdkAnchor(void);

const char *Grapple_SdkAnchor(void)
{
    return "grapple-beam";
}
