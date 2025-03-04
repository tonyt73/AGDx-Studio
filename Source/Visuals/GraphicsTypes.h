//---------------------------------------------------------------------------
#ifndef GraphicsTypesH
#define GraphicsTypesH
//---------------------------------------------------------------------------
#include <vector>
#include <System.Types.hpp>
//---------------------------------------------------------------------------
namespace Visuals
{
//---------------------------------------------------------------------------
// btBitmap         the buffer represents a bitmap of pixels; from 1 bit (monochrome) to 2,4 or 8 bits (paletted colour).
// btAttribute      the buffer represents the color values for the bitmap of pixels. Usually in 8x8 or 8x1 grouping.
// btCharacterMap   the buffer represents a character display similar to the MSX
enum BufferType  { btBitmap = 0, btAttribute = 1, btULAplus = 2, btCharacterMap = 3, btInvalid = -1     };
enum ImageTypes  { itStart = 1, itObject = 1, itSprite, itTile, itCharacterSet, itEnd, itAll, itInvalid };
enum ObjectState { osRoom, osDisabled, osInventory                                                      };
// a typical byte buffer
typedef std::vector<unsigned char>  ByteBuffer;
// Imaging sizing information
struct ImageSizing
{
    TSize   Minimum;
    TSize   Maximum;
    TSize   Step;
};
// TColor => TRGBA
struct TRGBA
{
    unsigned char R;
    unsigned char G;
    unsigned char B;
    unsigned char A;
};
//---------------------------------------------------------------------------
String     __fastcall ImageTypeName(ImageTypes type);
String     __fastcall BufferTypeName(BufferType type);
BufferType __fastcall BufferTypeByName(String name);
//---------------------------------------------------------------------------
} // Visuals namespace
//---------------------------------------------------------------------------
#endif // GraphicsTypesH
