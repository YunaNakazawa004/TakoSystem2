//==================================================================================
// 
// ファイルに関する処理[file.h]
// Author : Soki Inamura 
// 
//==================================================================================

#ifndef _FILE_H_
#define _FILE_H_

// プロトタイプ宣言 ============================================

bool FileExtractText			// ファイルから文字だけを読み取る処理
(FILE* pFile, char* pReadText);				// ファイルポインタ, 読み取った文字	

void FileMovePosion				// posの移動+保存処理
(const char* pSaveFilename,					// ファイル名
 D3DXVECTOR3* pPos, float fSpeed,			// 位置, 移動速度
 int nRightKey, int nLeftKey, 				// x方向移動キー(+,-)
 int nUpKey, int nDownKey, 					// y方向移動キー(+,-)
 int nForKey, int nBackKey);				// z方向移動キー(+,-)


void FileLogPass
(const char* pPassPointName);
#endif
