#ifndef GUARD_ACE_UTIL_BITMAP_H
#define GUARD_ACE_UTIL_BITMAP_H

#include <ace/types.h>

// File has its own 'flags' field - could be used in new ACE bitmap struct
#define BITMAP_INTERLEAVED 1
// FEATURE PROPOSAL:
// If set, ubBpp shows how many bitplanes are in bitmap, but after last one
// there is mask attached. Could be useful with c2p transforms - mask could be
// rotated with bitmap. Mask would be attached and detached with
// bitmapAttachMask() and bitmapDetachMask() fns.
#define BITMAP_MASK_ATTACHED 2

/* Types */

#ifdef AMIGA
#include <clib/graphics_protos.h> // BitMap etc
typedef struct BitMap tBitMap;
#else
typedef struct _tBitMap {
	UWORD BytesPerRow;
	UWORD Rows;
	UBYTE Flags;
	UBYTE Depth;
	UWORD pad;
	UWORD *Planes[8];
} tBitMap;
#define BMF_CLEAR       (1 << 0)
#define BMF_DISPLAYABLE (1 << 1)
#define BMF_INTERLEAVED (1 << 2)
#define BMF_STANDARD    (1 << 3)
#define BMF_MINPLANES   (1 << 4)
#endif // AMIGA

/**
 * @brief New bitmap format.
 * Don't use until adopted into entire engine - this struct is more like feature
 * request or memo.
 */
typedef struct _tAceBitmap {
	UWORD uwWidth; ///< Actual width, in pixels.
	UWORD uwHeight;
	UWORD uwBytesPerRow; ///< Useful during raw ops, faster than (uwWidth+15)>>3.
	                     /// Perhaps uwWordsPerRow would be more useful?
	UBYTE ubBpp;
	UBYTE ubFlags; ///< Interleaved or not
	UWORD *pPlanes[8]; ///< OCS uses up to 6, AGA up to 8
} tAceBitmap;

/* Globals */

/* Functions */

/**
 *  @brief Allocates bitmap of given dimensions and depth.
 *  OS' AllocBitMap is not present on KS1.3, hence this OS-compatible
 *  implementation.
 *
 *  @param uwWidth  Desired bitmap width, in pixels.
 *  @param uwHeight Desired bitmap height, in pixels.
 *  @param ubDepth  Desired bitmap depth (bits per pixel)
 *  @param ubFlags  Bitmap creation flags, see BMF_* defines
 *  @return Newly created OS-compatible bitmap, 0 on error.
 *
 *  @see bitmapDestroy
 *  @see bitmapCreateFromFile
 *  @see bitmapLoadFromFile
 */
tBitMap* bitmapCreate(
	IN UWORD uwWidth,
	IN UWORD uwHeight,
	IN UBYTE ubDepth,
	IN UBYTE ubFlags
);

/**
 *  @brief Loads bitmap data from file to already existing bitmap.
 *  If source is smaller than destination, you can use uwStartX & uwStartY
 *  params to load bitmap on given coords.
 *
 *  @param pBitMap    Pointer to destination bitmap
 *  @param szFilePath Source bitmap file path.
 *  @param uwStartX   Start X-coordinate on destination bitmap, 8-pixel aligned.
 *  @param uwStartY   Start Y-coordinate on destination bitmap
 *
 *  @see bitmapCreate
 *  @see bitmapCreateFromFile
 */
void bitmapLoadFromFile(
	IN tBitMap *pBitMap,
	IN char *szFilePath,
	IN UWORD uwStartX,
	IN UWORD uwStartY
);

/**
 *  @brief Creates bitmap and loads its data from file.
 *  As opposed to bitmapLoadFromFile, this function creates bitmap based
 *  on dimensions, BPP & flags stored in file.
 *
 *  @param szFilePath Source bitmap file path.
 *  @return Pointer to newly created bitmap based on file, 0 on error.
 *
 *  @see bitmapLoadFromFile
 *  @see bitmapCreate
 *  @see bitmapDestroy
 */
tBitMap* bitmapCreateFromFile(
	IN char *szFileName
);

/**
 *  @brief Destroys given bitmap, freeing its resources to OS.
 *  Be sure to end all blitter & display operations on this bitmap
 *  prior to calling this function.
 *
 *  @param pBitMap Bitmap to be destroyed.
 *
 *  @see bitmapCreate
 *  @see bitmapCreateFromFile
 */
void bitmapDestroy(
	IN tBitMap *pBitMap
);

/**
 *  @brief Checks if given bitmap is interleaved.
 *  Detection should work on any OS bitmap.
 *
 *  @param pBitMap Bitmap to be checked.
 *  @return non-zero if bitmap is interleaved, otherwise zero.
 */
BYTE bitmapIsInterleaved(
	IN tBitMap *pBitMap
);

/**
 *  @brief Saves basic Bitmap information to log file.
 *
 *  @param pBitMap Bitmap to be dumped.
 *
 *  @see bitmapSaveBMP
 */
void bitmapDump(
	IN tBitMap *pBitMap
);

/**
 * Saves bitmap in ACE .bm format.
 * @param pBitMap Bitmap to be saved.
 * @param szPath  Path of destination file.
 */
void bitmapSave(
	IN tBitMap *pBitMap,
	IN char *szPath
);

/**
 *  @brief Saves given Bitmap as BMP file.
 *  Use only for debug purposes, as conversion is outrageously slow.
 *
 *  @param pBitMap    Bitmap to be dumped.
 *  @param pPalette   Palette to be used during export.
 *  @param szFilePath Destination file path.
 */
void bitmapSaveBmp(
	IN tBitMap *pBitMap,
	IN UWORD *pPalette,
	IN char *szFileName
);

/**
 *  @brief Returns bitmap width in bytes.
 *  Direct check to BytesPerRow may lead to errors as in interleaved mode it
 *  stores value multiplied by bitplane count.
 */
UWORD bitmapGetByteWidth(tBitMap *pBitMap);

#endif // GUARD_ACE_UTIL_BITMAP_H
