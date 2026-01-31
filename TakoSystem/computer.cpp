//=============================================================================
// 
// CPU [computer.cpp]
// Author : 中澤優奈
// 
//=============================================================================
#include "computer.h"
#include "esa.h"
#include "pot.h"
#include "time.h"
#include "meshcylinder.h"
#include "effect_3d.h"
#include "camera.h"
#include "input.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MOVEMENT				(D3DXVECTOR3(30.0f, 5.0f, 30.0f))		// 移動量
#define TENTACLE_MOVEMENT		(D3DXVECTOR3(40.0f, 7.0f, 40.0f))		// 触手移動の移動量
#define ROT						(D3DXVECTOR3(0.05f, 0.05f, 0.05f))		// 向き移動量
#define INERTIA_MOVE			(0.2f)									// 移動の慣性
#define DASH_MOVE				(0.01f)									// 高速移動の速さ
#define DASH_REACH				(10.0f)									// 高速移動のリーチ
#define MAX_MOVE				(10.0f)									// 移動の制限
#define INERTIA_ANGLE			(0.1f)									// 角度の慣性
#define POS_ERROR				(10.0f)									// 位置の誤差
#define TENTACLE_RANGE			(DISTANCE * DASH_REACH)					// 触手のリーチ
#define CPU_TENTACLE			(8)										// 足の数

#define FAR_DISTANCE			(6000.0f)								// 遠すぎる
#define DISTANCE_SCORE			(0.001f)								// 距離スコアの減衰
#define NEAR_WALL_DISTANCE		(1500.0f)								// 壁際の距離
#define NEAR_PILLAR_DISTANCE	(2000.0f)								// 柱に近いかの距離

#define ESA_DOT_SCORE			(3.0f)									// エサの進行方向スコア
#define ESA_ENEMY_DISTANCE		(2500.0f)								// 近い敵との距離
#define ESA_ENEMY_DIST_SCORE	(4.0f)									// 近い敵のスコア
#define ESA_PILLAR_SCORE		(2.0f)									// 柱のスコア
#define ESA_WATER_SCORE			(1.0f)									// 浮いているエサのスコア
#define ESA_NEAR_WALL_SCORE		(2.0f)									// 壁際のエサのスコア

#define ENEMY_DOT_SCORE			(4.0f)									// 敵が進行方向にいるスコア
#define ENEMY_FAR_WALL_SCORE	(3.0f)									// 敵が壁から離れているスコア
#define ENEMY_PILLAR_SCORE		(2.0f)									// 敵が柱の裏にいないスコア
#define ENEMY_BLIND_SCORE		(3.0f)									// 敵の視界が悪いスコア
#define	ENEMY_TENTACLE_SCORE	(2.0f)									// 敵の触手がクールダウン中のスコア
#define ENEMY_COUNT_SCORE		(5.0f)									// 敵が複数いるときのスコア
#define ENEMY_COUNT_DIST		(3000.0f)								// 敵が複数いるときの距離

#define ESCAPE_ENEMY_DIST		(8000.0f)								// 遠すぎる敵とは判定しない
#define ESCAPE_DOT				(-0.3f)									// 逃げるかの角度
#define	ESCAPE_DOT_SCORE		(6.0f)									// 追われているときのスコア
#define ESCAPE_ENEMY_MOVE		(5.0f)									// 敵の速度が速いときのスコア
#define ESCAPE_ENEMY_SCORE		(5.0f)									// 複数の敵がいるときのスコア
#define ESCAPE_ESA_COUNT		(0.4f)									// 自分のエサが多い
#define ESCAPE_PILLAR_SCORE		(3.0f)									// 柱が近いときのスコア
#define	ESCAPE_TENTACLE_SCORE	(3.0f)									// 敵の触手がクールダウンしていないスコア
#define ESCAPE_INK_COUNT		(30)									// 敵が墨を吐いてからの経過フレーム
#define ESCAPE_INK_SCORE		(2.0f)									// 敵が墨を吐いた後のスコア

#define INK_ENEMY_MOVE			(4.0f)									// 敵の速度が速い時のスコア
#define INK_ENEMY_ESA_COUNT		(0.3f)									// 敵の持っているエサが多い
#define INK_ENEMY_COUNT_SCORE	(5.0f)									// 敵が複数いるときのスコア
#define INK_ESA_COUNT			(0.2f)									// 自分のエサが多い
#define INK_PILLAR_SCORE		(3.0f)									// 柱が近いスコア

#define POT_DISTANCE			(3000.0f)								// タコつぼが遠すぎる
#define POT_ESA_SCORE			(0.4f)									// 自分のエサが多い
#define POT_ENEMY_DISTANCE		(3000.0f)								// 敵との距離
#define POT_ENEMY_DIST_SCORE	(3.0f)									// 敵との距離が遠いスコア
#define POT_PHASE_1				(30)									// タコつぼ優先になる時間第一段階
#define POT_PHASE_2				(10)									// タコつぼ優先になる時間第二段階
#define POT_PHASE_1_SCORE		(10.0f)									// 第一段階のスコア
#define POT_PHASE_2_SCORE		(20.0f)									// 第二段階のスコア
#define POT_CLOSE_DISTANCE		(800.0f)								// タコつぼと近い

#define EXPLORE_ESA				(3)										// エサを探す
#define EXPLORE_LITTLE_ESA		(6)										// 少しエサを探す
#define EXPLORE_THRESHOLD		(3.0f)									// 探索するための閾値
#define EXPLORE_LIT_THRESHOLD	(4.0f)									// 少し探索するための閾値

#define RAY_COUNT				(16)									// レイキャストを伸ばす方向
#define NODE_COUNT				(16)									// ノードの円分割
#define NODE_HEIGHT				(3)										// ノードの縦分割

#define INK_COOLDOWN			(300)									// 墨吐きクールダウン
#define TENTACLE_COOLDOWN		(60)									// 触手移動クールダウン
#define CPU_THINK				(5)										// 思考間隔
#define CPU_WIDTH				(5.0f)									// 幅
#define CPU_HEIGHT				(10.0f)									// 高さ
#define CPU_FILE				"data\\motion_octo_1.txt"				// CPUのデータファイル

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void LoadComputer(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
Computer g_aComputer[MAX_COMPUTER];						// CPUの情報
Node g_aOutNode[NODE_COUNT * NODE_HEIGHT];				// ノード(外周)
Node g_aInNode[NODE_COUNT * NODE_HEIGHT];				// ノード(内周)
char* g_apFilenameComputer[MAX_NUMMODEL] = {};			// モデルファイルへのポインタ

//=============================================================================
// CPUの初期化処理
//=============================================================================
void InitComputer(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ
	D3DXMATERIAL* pMat;
	Computer* pComputer = GetComputer();

	// CPUの情報の初期化
	for (int nCntComputer = 0; nCntComputer < MAX_COMPUTER; nCntComputer++, pComputer++)
	{
		pComputer->nIdx = nCntComputer;
		pComputer->state = CPUSTATE_APPEAR;
		pComputer->nCounterState = 0;
		pComputer->phys.pos = FIRST_POS;
		pComputer->phys.posOld = FIRST_POS;
		pComputer->phys.move = FIRST_POS;
		pComputer->phys.rot = FIRST_POS;
		pComputer->phys.dir = FIRST_POS;
		pComputer->phys.fAngle = 0.0f;
		pComputer->phys.fRadius = CPU_WIDTH;
		pComputer->phys.fHeight = CPU_HEIGHT;
		pComputer->bUse = false;

		pComputer->nTargetFoodIdx = -1;
		pComputer->nTargetEnemyIdx = -1;
		pComputer->nTargetPotIdx = -1;
		pComputer->targetWall = FIRST_POS;
		pComputer->extarget = FIRST_POS;
		pComputer->posX = FIRST_POS;
		pComputer->vecX = FIRST_POS;

		pComputer->fFoodScore = 0;
		pComputer->fAttackScore = 0;
		pComputer->fEscapeScore = 0;
		pComputer->fInkScore = 0;

		pComputer->nTentacleCooldown = 0;
		pComputer->nInkCooldown = 0;
		pComputer->nThinkCooldown = 0;

		pComputer->bBlinded = false;
		pComputer->nBlindCounter = 0;

		pComputer->nFoodCount = 0;
		pComputer->nMaxFood = 1;

		pComputer->nCurrentNode = 0;
		pComputer->nNextNode = 0;
	}

	// ファイルからキャラクター情報をロード
	LoadComputer();

	pComputer = GetComputer();

	for (int nCntComputer = 0; nCntComputer < MAX_COMPUTER; nCntComputer++, pComputer++)
	{
		pComputer->motionType = MOTIONTYPE_NEUTRAL;
		pComputer->bLoopMotion = pComputer->aMotionInfo[0].bLoop;
		pComputer->nNumKey = pComputer->aMotionInfo[0].nNumKey;
		pComputer->nKey = 0;
		pComputer->nCounterMotion = 0;
		pComputer->bFinishMotion = false;
		pComputer->bBlendMotion = false;
		pComputer->motionTypeBlend = MOTIONTYPE_NEUTRAL;
		pComputer->bLoopMotionBlend = pComputer->aMotionInfo[0].bLoop;
		pComputer->nNumKeyBlend = pComputer->aMotionInfo[0].nNumKey;
		pComputer->nKeyBlend = 0;
		pComputer->nCounterMotionBlend = 0;
		pComputer->nFrameBlend = 0;
		pComputer->nCounterBlend = 0;

		for (int nCntModel = 0; nCntModel < pComputer->nNumModel; nCntModel++)
		{
			// Xファイルの読み込み
			if (FAILED(D3DXLoadMeshFromX(g_apFilenameComputer[pComputer->aModel[nCntModel].nIdx],
				D3DXMESH_SYSTEMMEM,
				pDevice,
				NULL,
				&pComputer->aModel[nCntModel].pBuffMat,
				NULL,
				&pComputer->aModel[nCntModel].dwNumMat,
				&pComputer->aModel[nCntModel].pMesh)))
			{
				return;
			}

			// マテリアルデータへのポインタを取得
			pMat = (D3DXMATERIAL*)pComputer->aModel[nCntModel].pBuffMat->GetBufferPointer();

			for (int nCntMat = 0; nCntMat < (int)pComputer->aModel[nCntModel].dwNumMat; nCntMat++)
			{
				if (pMat[nCntMat].pTextureFilename != NULL)
				{// テクスチャファイルが存在する
					D3DXCreateTextureFromFile(pDevice, pMat[nCntMat].pTextureFilename, &pComputer->aModel[nCntModel].apTexture[nCntMat]);
				}
			}
		}

		SetMotionComputer(nCntComputer, MOTIONTYPE_NEUTRAL, false, 0);
	}

	// ランダムな位置に設定
	SetRandomComputer(10);
	//SetComputer(D3DXVECTOR3(0.0f, 10000.0f, 4000.0f), FIRST_POS);

	// ノードの設置
	CreateOuterNodes3D();
	CreateInnerNodes3D();
}

//=============================================================================
// CPUの終了処理
//=============================================================================
void UninitComputer(void)
{
	Computer* pComputer = GetComputer();

	for (int nCntModel = 0; nCntModel < pComputer->nNumModel; nCntModel++)
	{
		// メッシュの破棄
		if (pComputer->aModel[nCntModel].pMesh != NULL)
		{
			pComputer->aModel[nCntModel].pMesh->Release();
			pComputer->aModel[nCntModel].pMesh = NULL;
		}

		// マテリアルの破棄
		if (pComputer->aModel[nCntModel].pBuffMat != NULL)
		{
			pComputer->aModel[nCntModel].pBuffMat->Release();
			pComputer->aModel[nCntModel].pBuffMat = NULL;
		}

		// テクスチャの破棄
		for (int nCntComputer = 0; nCntComputer < (int)pComputer->aModel[nCntModel].dwNumMat; nCntComputer++)
		{
			if (pComputer->aModel[nCntModel].apTexture[nCntComputer] != NULL)
			{
				pComputer->aModel[nCntModel].apTexture[nCntComputer]->Release();
				pComputer->aModel[nCntModel].apTexture[nCntComputer] = NULL;
			}
		}
	}
}

//=============================================================================
// CPUの更新処理
//=============================================================================
void UpdateComputer(void)
{
	Camera* pCamera = GetCamera();
	Computer* pComputer = GetComputer();
	float fmoveAngle = 0.0f;

	for (int nCntComputer = 0; nCntComputer < MAX_COMPUTER; nCntComputer++, pComputer++)
	{
		if (pComputer->bUse == true)
		{
			if (pComputer->nThinkCooldown > 0)
			{// 思考クールダウン
				pComputer->nThinkCooldown--;
			}
			else
			{// 5フレームに1回判断
				pComputer->nThinkCooldown = CPU_THINK;

				// スコア計算
				CalcFoodScore(pComputer);
				CalcAttackScore(pComputer);
				CalcEscapeScore(pComputer);
				CalcInkScore(pComputer);
				CalcPotScore(pComputer);

				// 最優先行動を決定
				float best = pComputer->fFoodScore;
				pComputer->state = CPUSTATE_MOVE_TO_FOOD;

				if (pComputer->fAttackScore > best)
				{// 攻撃
					best = pComputer->fAttackScore;
					pComputer->state = CPUSTATE_ATTACK;
				}

				if (pComputer->fEscapeScore > best)
				{// 回避
					best = pComputer->fEscapeScore;
					pComputer->state = CPUSTATE_ESCAPE;
				}

				if (pComputer->fInkScore > best)
				{// 墨吐き
					best = pComputer->fInkScore;
					pComputer->state = CPUSTATE_INK_ATTACK;
				}

				if (pComputer->fPotScore > best)
				{// タコつぼ
					best = pComputer->fPotScore;
					pComputer->state = CPUSTATE_GO_TO_POT;
				}

				float exploreThreshold = EXPLORE_THRESHOLD;


				if (pComputer->nFoodCount < EXPLORE_ESA)
				{// エサが少ないほど探索しやすくする
					exploreThreshold = EXPLORE_THRESHOLD * 2.0f;
				}
				else if (pComputer->nFoodCount < EXPLORE_LITTLE_ESA)
				{// 少し探索しやすい
					exploreThreshold = EXPLORE_LIT_THRESHOLD;
				}
				else
				{// 通常
					exploreThreshold = EXPLORE_THRESHOLD;
				}

				if (best < EXPLORE_THRESHOLD)
				{// 閾値以下なら探索
					pComputer->state = CPUSTATE_EXPLORE;
				}
			}

			PrintDebugProc("ENEMY : [ %d ]\n", nCntComputer);

			pComputer->phys.posOld = pComputer->phys.pos;

			if (D3DXVec3Length(&pComputer->phys.move) > 0.1f)
			{// 方向ベクトル
				D3DXVec3Normalize(&pComputer->phys.dir, &pComputer->phys.move);
			}

			switch (pComputer->state)
			{
			case CPUSTATE_APPEAR:				// 出現状態
				PrintDebugProc("CPUの状態 : [ CPUSTATE_APPEAR ]\n");

				break;

			case CPUSTATE_EXPLORE:				// 探索
				PrintDebugProc("CPUの状態 : [ CPUSTATE_EXPLORE ]\n");

				Explore(pComputer);

				break;

			case CPUSTATE_MOVE_TO_FOOD:			// エサを取りに行く
				PrintDebugProc("CPUの状態 : [ CPUSTATE_MOVE_TO_FOOD ]\n");

				MoveToFood(pComputer);

				break;

			case CPUSTATE_ESCAPE:				// 回避
				PrintDebugProc("CPUの状態 : [ CPUSTATE_ESCAPE ]\n");

				if (ShouldUseTentacle(pComputer) == true)
				{// 触手移動
					FindTentacleTarget(pComputer);
					UseTentacle(pComputer);
				}

				Escape(pComputer);

				break;

			case CPUSTATE_ATTACK:				// 敵を追う
				PrintDebugProc("CPUの状態 : [ CPUSTATE_ATTACK ]\n");

				if (ShouldUseTentacle(pComputer) == true)
				{// 触手移動
					FindTentacleTarget(pComputer);
					UseTentacle(pComputer);
				}

				AttackEnemy(pComputer);

				break;

			case CPUSTATE_INK_ATTACK:			// 墨吐き
				PrintDebugProc("CPUの状態 : [ CPUSTATE_INK_ATTACK ]\n");

				InkAttack(pComputer);

				break;

			case CPUSTATE_HIDE:					// 柱の裏に隠れる
				PrintDebugProc("CPUの状態 : [ CPUSTATE_HIDE ]\n");

				HideBehindPillar(pComputer);

				break;

			case CPUSTATE_WAIT:					// 待機
				PrintDebugProc("CPUの状態 : [ CPUSTATE_WAIT ]\n");

				break;

			case CPUSTATE_GO_TO_POT:			// タコつぼへ向かう
				PrintDebugProc("CPUの状態 : [ CPUSTATE_GO_TO_POT ]\n");

				if (ShouldUseTentacle(pComputer) == true)
				{// 触手移動
					FindTentacleTarget(pComputer);
					UseTentacle(pComputer);
				}

				GoToPot(pComputer);

				break;

			case CPUSTATE_HIDE_FOOD:			// エサを隠す
				PrintDebugProc("CPUの状態 : [ CPUSTATE_HIDE_FOOD ]\n");

				HideFood(pComputer);

				break;

			case CPUSTATE_STEAL_FOOD:			// 隠されたエサを奪う
				PrintDebugProc("CPUの状態 : [ CPUSTATE_STEAL_FOOD ]\n");

				StealFood(pComputer);

				break;

			case CPUSTATE_FINAL_COLLECT:		// 終盤の回収行動
				PrintDebugProc("CPUの状態 : [ CPUSTATE_FINAL_COLLECT ]\n");

				if (ShouldUseTentacle(pComputer) == true)
				{// 触手移動
					FindTentacleTarget(pComputer);
					UseTentacle(pComputer);
				}

				FinalCollect(pComputer);

				break;
			}

			// 移動量制限
			if (pComputer->phys.move.x > MAX_MOVE)
			{// 最大X
				pComputer->phys.move.x = MAX_MOVE;
			}
			else if (pComputer->phys.move.x < -MAX_MOVE)
			{// 最小X
				pComputer->phys.move.x = -MAX_MOVE;
			}

			if (pComputer->phys.move.z > MAX_MOVE)
			{// 最大Z
				pComputer->phys.move.z = MAX_MOVE;
			}
			else if (pComputer->phys.move.z < -MAX_MOVE)
			{// 最小Z
				pComputer->phys.move.z = -MAX_MOVE;
			}

			if (pComputer->state != CPUSTATE_APPEAR)
			{// 出現状態以外
				// 慣性
				pComputer->phys.pos += pComputer->phys.move;
				pComputer->phys.move.x += (0.0f - pComputer->phys.move.x) * INERTIA_MOVE;
				pComputer->phys.move.y += (0.0f - pComputer->phys.move.y) * INERTIA_MOVE;
				pComputer->phys.move.z += (0.0f - pComputer->phys.move.z) * INERTIA_MOVE;
			}

			// 移動制限
			if (pComputer->phys.pos.x < -ALLOW_X)
			{// 一番左
				pComputer->phys.pos.x = -ALLOW_X;
			}
			else if (pComputer->phys.pos.x > ALLOW_X)
			{// 一番右
				pComputer->phys.pos.x = ALLOW_X;
			}

			if (pComputer->phys.pos.z < -ALLOW_Z)
			{// 一番奥
				pComputer->phys.pos.z = -ALLOW_Z;
			}
			else if (pComputer->phys.pos.z > ALLOW_Z)
			{// 一番手前
				pComputer->phys.pos.z = ALLOW_Z;
			}

			if (pComputer->phys.pos.y < 0.0f)
			{// 底
				pComputer->phys.pos.y = 0.0f;
			}

			fmoveAngle = pComputer->phys.fAngle - pComputer->phys.rot.y;

			// 向きを調整
			CorrectAngle(&pComputer->phys.fAngle, fmoveAngle);

			if (pComputer->phys.rot.y != pComputer->phys.fAngle)
			{// 目標地点につくまで慣性で角度を足す
				pComputer->phys.rot.y += (pComputer->phys.fAngle - pComputer->phys.rot.y) * INERTIA_ANGLE;

				// 向きを調整
				CorrectAngle(&pComputer->phys.rot.y, pComputer->phys.rot.y);
			}

			//SetEffect3D(70, pComputer->phys.pos, D3DXVECTOR3(0.0f, 0.0f, 0.0f), 0.0f, 30.0f, -0.1f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

			//PrintDebugProc("ENEMY : pos ( %f %f %f )\n",
			//	pComputer->phys.pos.x, pComputer->phys.pos.y, pComputer->phys.pos.z);
			//PrintDebugProc("ENEMY : move ( %f %f %f )\n",
			//	pComputer->phys.move.x, pComputer->phys.move.y, pComputer->phys.move.z);
			PrintDebugProc("ENEMY : nFood ( %d )\n", pComputer->nFoodCount);
			//PrintDebugProc("ENEMY : ノード ( %f %f %f )\n",
			//	pComputer->extarget.x, pComputer->extarget.y, pComputer->extarget.z);

			CollisionPot(&pComputer->phys.pos, &pComputer->phys.posOld, &pComputer->phys.move, pComputer->phys.fRadius, pComputer->phys.fRadius);

			D3DXVECTOR3 posAway;
			posAway.x = pComputer->phys.pos.x + sinf(D3DX_PI + pComputer->phys.rot.y) * 10000.0f;
			posAway.y = pComputer->phys.pos.y;
			posAway.z = pComputer->phys.pos.z + cosf(D3DX_PI - pComputer->phys.rot.y) * 10000.0f;

			CollisionMeshCylinder(&posAway, &pComputer->phys.pos, &pComputer->phys.move, pComputer->phys.fRadius, pComputer->phys.fRadius, true);

			// 当たり判定
			CollisionMeshCylinder(&pComputer->phys.pos, &pComputer->phys.posOld, &pComputer->phys.move, pComputer->phys.fRadius, pComputer->phys.fRadius, false);

			// モーションの更新処理
			UpdateMotionComputer();
		}
	}
}

//=============================================================================
// CPUの描画処理
//=============================================================================
void DrawComputer(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ
	D3DXMATRIX mtxRot, mtxTrans;		// 計算用マトリックス
	D3DMATERIAL9 matDef;				// 現在のマテリアル保存用
	D3DXMATERIAL* pMat;					// マテリアルデータへのポインタ
	Computer* pComputer = GetComputer();

	for (int nCntComputer = 0; nCntComputer < MAX_COMPUTER; nCntComputer++, pComputer++)
	{
		if (pComputer->bUse == true)
		{// 使用しているとき
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&pComputer->phys.mtxWorld);

			// 向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, pComputer->phys.rot.y, pComputer->phys.rot.x, pComputer->phys.rot.z);
			D3DXMatrixMultiply(&pComputer->phys.mtxWorld, &pComputer->phys.mtxWorld, &mtxRot);

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, pComputer->phys.pos.x, pComputer->phys.pos.y, pComputer->phys.pos.z);
			D3DXMatrixMultiply(&pComputer->phys.mtxWorld, &pComputer->phys.mtxWorld, &mtxTrans);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &pComputer->phys.mtxWorld);

			// 現在のマテリアルを取得
			pDevice->GetMaterial(&matDef);

			// 全モデル(パーツ)の描画
			for (int nCntModel = 0; nCntModel < pComputer->nNumModel; nCntModel++)
			{
				D3DXMATRIX mtxRotModel, mtxTransModel;		// 計算用マトリックス
				D3DXMATRIX mtxParent;						// 親のマトリックス

				// パーツのワールドマトリックスの初期化
				D3DXMatrixIdentity(&pComputer->aModel[nCntModel].mtxWorld);

				// パーツの向きを反映
				D3DXMatrixRotationYawPitchRoll(&mtxRotModel, pComputer->aModel[nCntModel].rot.y, pComputer->aModel[nCntModel].rot.x, pComputer->aModel[nCntModel].rot.z);
				D3DXMatrixMultiply(&pComputer->aModel[nCntModel].mtxWorld, &pComputer->aModel[nCntModel].mtxWorld, &mtxRotModel);

				// パーツの位置を反映
				D3DXMatrixTranslation(&mtxTransModel, pComputer->aModel[nCntModel].pos.x, pComputer->aModel[nCntModel].pos.y, pComputer->aModel[nCntModel].pos.z);
				D3DXMatrixMultiply(&pComputer->aModel[nCntModel].mtxWorld, &pComputer->aModel[nCntModel].mtxWorld, &mtxTransModel);

				// パーツの「親のマトリックス」を設定
				if (pComputer->aModel[nCntModel].nIdxModelParent != -1)
				{// 親モデルがある場合
					mtxParent = pComputer->aModel[pComputer->aModel[nCntModel].nIdxModelParent].mtxWorld;
				}
				else
				{// 親モデルがない場合
					mtxParent = pComputer->phys.mtxWorld;
				}

				// 算出した「パーツのワールドマトリックス」と「親のマトリックス」をかけ合わせる
				D3DXMatrixMultiply(&pComputer->aModel[nCntModel].mtxWorld,
					&pComputer->aModel[nCntModel].mtxWorld,
					&mtxParent);

				// パーツのワールドマトリックスを設定
				pDevice->SetTransform(D3DTS_WORLD, &pComputer->aModel[nCntModel].mtxWorld);

				// マテリアルデータへのポインタを取得
				pMat = (D3DXMATERIAL*)pComputer->aModel[nCntModel].pBuffMat->GetBufferPointer();

				for (int nCntMat = 0; nCntMat < (int)pComputer->aModel[nCntModel].dwNumMat; nCntMat++)
				{
					// マテリアルの設定
					pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

					// テクスチャの設定
					pDevice->SetTexture(0, pComputer->aModel[nCntModel].apTexture[nCntMat]);

					// モデルパーツの描画
					pComputer->aModel[nCntModel].pMesh->DrawSubset(nCntMat);
				}
			}

			// 保存していたマテリアルを戻す
			pDevice->SetMaterial(&matDef);
		}
	}
}

//=============================================================================
// エサを取りに行く
//=============================================================================
void MoveToFood(Computer* pComputer)
{
	if (pComputer->nTargetFoodIdx < 0)
	{// どのエサもターゲットしてない
		return;
	}

	PrintDebugProc("ENEMY : targetFood ( %d )\n", pComputer->nTargetFoodIdx);

	D3DXVECTOR3 target = GetFoodPosition(pComputer);
	D3DXVECTOR3 dir = target - pComputer->phys.pos;
	D3DXVec3Normalize(&dir, &dir);

	// 慣性移動
	pComputer->phys.move.x += dir.x * MOVEMENT.x;
	pComputer->phys.move.y += dir.y * MOVEMENT.y;
	pComputer->phys.move.z += dir.z * MOVEMENT.z;

	int nIdx = -1;

	if (CollisionEsa(&nIdx, false, &pComputer->phys.pos, pComputer->phys.fRadius) == true)
	{// エサと接触した
		Esa* pEsa = GetEsa();
		pEsa[nIdx].bUse = false;

		pComputer->nFoodCount++;
	}
}

//=============================================================================
// 攻撃
//=============================================================================
void AttackEnemy(Computer* pComputer)
{
	if (pComputer->nTargetEnemyIdx < 0)
	{// どの敵もターゲットしてない
		return;
	}

	Camera* pCamera = GetCamera();

	D3DXVECTOR3 target = GetEnemyPosition(pComputer);
	D3DXVECTOR3 dir = target - pComputer->phys.pos;
	D3DXVec3Normalize(&dir, &dir);

	// 慣性移動
	pComputer->phys.move.x += dir.x * MOVEMENT.x;
	pComputer->phys.move.y += dir.y * MOVEMENT.y;
	pComputer->phys.move.z += dir.z * MOVEMENT.z;

	D3DXVECTOR3 posDiff = target - pComputer->phys.pos;

	// 触手射程内なら掴みに行く
	if (D3DXVec3Length(&posDiff) < pCamera->fDistance * DASH_REACH) {
		FindTentacleTarget(pComputer);
		UseTentacle(pComputer);
	}
}

//=============================================================================
// 逃げる
//=============================================================================
void Escape(Computer* pComputer)
{
	D3DXVECTOR3 enemyPos = GetNearestEnemy(pComputer);
	D3DXVECTOR3 dir = pComputer->phys.pos - enemyPos; // 敵と逆方向
	D3DXVec3Normalize(&dir, &dir);

	// 慣性移動
	pComputer->phys.move.x += dir.x * MOVEMENT.x;
	pComputer->phys.move.y += dir.y * MOVEMENT.y;
	pComputer->phys.move.z += dir.z * MOVEMENT.z;

	if (IsNearPillar(pComputer->phys.pos))
	{// 柱が近いなら隠れる状態へ
		pComputer->state = CPUSTATE_HIDE;
	}
}

//=============================================================================
// 柱に隠れる
//=============================================================================
void HideBehindPillar(Computer* pComputer)
{
	//D3DXVECTOR3 safePos = GetOppositeSideOfPillar(pComputer->phys.pos);
	//D3DXVECTOR3 dir = safePos - pComputer->phys.pos;
	//D3DXVec3Normalize(&dir, &dir);

	//// 慣性移動
	//pComputer->phys.move.x += dir.x * MOVEMENT.x;
	//pComputer->phys.move.y += dir.y * MOVEMENT.y;
	//pComputer->phys.move.z += dir.z * MOVEMENT.z;
}

//=============================================================================
// 墨吐き
//=============================================================================
void InkAttack(Computer* pComputer)
{
	if (pComputer->nInkCooldown > 0)
	{// クールダウン中
		return;
	}

	// 墨を吐く処理
	//EmitInk(pComputer); 

	pComputer->nInkCooldown = INK_COOLDOWN;

	// 墨を吐いた後は逃げる or 攻撃に戻る
	pComputer->state = (pComputer->fEscapeScore > pComputer->fAttackScore) ? CPUSTATE_ESCAPE : CPUSTATE_ATTACK;
}

//=============================================================================
// エサを探す
//=============================================================================
void Explore(Computer* pComputer)
{
	D3DXVECTOR3 posDiff = pComputer->extarget - pComputer->phys.pos;
	float fDist = D3DXVec3Length(&posDiff);
	static int nCounterThink = 0;
	nCounterThink++;

	if (nCounterThink % (ONE_SECOND * 5) == 0)
	{// 5秒に1回設定
		pComputer->extarget = GetRandomExplorePoint();
	}

	D3DXVECTOR3 dir = pComputer->extarget - pComputer->phys.pos;
	D3DXVec3Normalize(&dir, &dir);

	// 慣性移動
	pComputer->phys.move.x += dir.x * MOVEMENT.x;
	pComputer->phys.move.y += dir.y * MOVEMENT.y;
	pComputer->phys.move.z += dir.z * MOVEMENT.z;
}

//=============================================================================
// タコつぼに向かう
//=============================================================================
void GoToPot(Computer* pComputer)
{
	if (pComputer->nTargetPotIdx < 0)
	{// どのタコつぼもターゲットしてない
		return;
	}

	Pot* pPot = GetPot();

	D3DXVECTOR3 potPos = pPot[pComputer->nTargetPotIdx].pos;
	D3DXVECTOR3 dir = potPos - pComputer->phys.pos;
	D3DXVec3Normalize(&dir, &dir);

	// 慣性移動
	pComputer->phys.move.x += dir.x * MOVEMENT.x;
	pComputer->phys.move.y += dir.y * MOVEMENT.y;
	pComputer->phys.move.z += dir.z * MOVEMENT.z;

	D3DXVECTOR3 posDiff = potPos - pComputer->phys.pos;

	// 触手移動
	float dist = D3DXVec3Length(&posDiff);

	if (dist < TENTACLE_RANGE)
	{// 近いなら触手移動
		FindTentacleTarget(pComputer);
		UseTentacle(pComputer);
	}

	if (dist < POT_CLOSE_DISTANCE)
	{// タコつぼに到達したら次の状態へ
		pPot = &pPot[pComputer->nTargetPotIdx];

		if (pPot->nFood == 0)
		{// 何も入ってない
			pComputer->state = CPUSTATE_HIDE_FOOD;
		}
		else
		{// 何か入っている
			pComputer->state = CPUSTATE_STEAL_FOOD;
		}
	}
}

//=============================================================================
// エサを隠す
//=============================================================================
void HideFood(Computer* pComputer)
{
	if (pComputer->nTargetPotIdx < 0)
	{// どのタコつぼもターゲットしてない
		return;
	}

	Pot* pPot = GetPot();
	pPot = &pPot[pComputer->nTargetPotIdx];

	// 自分のエサをタコつぼに移す
	if (pComputer->nFoodCount > 0)
	{
		pPot->nFood += pComputer->nFoodCount;
		pComputer->nFoodCount = 0;
	}

	// 隠し終わったら探索へ戻る
	pComputer->state = CPUSTATE_EXPLORE;
}

//=============================================================================
// エサを奪う
//=============================================================================
void StealFood(Computer* pComputer)
{
	if (pComputer->nTargetPotIdx < 0)
	{// どのタコつぼもターゲットしてない
		return;
	}

	Pot* pPot = GetPot();
	pPot = &pPot[pComputer->nTargetPotIdx];

	// 触手でエサを奪う
	D3DXVECTOR3 posDiff = pPot->pos - pComputer->phys.pos;
	float dist = D3DXVec3Length(&posDiff);

	if (dist < TENTACLE_RANGE)
	{
		pComputer->nFoodCount += pPot->nFood;
		pPot->nFood = 0;

		if (pPot->nFood <= 0)
		{// 全部奪ったら探索へ
			pComputer->state = CPUSTATE_EXPLORE;

			return;
		}
	}

	// まだ距離が遠いなら近づく
	D3DXVECTOR3 dir = pPot->pos - pComputer->phys.pos;
	D3DXVec3Normalize(&dir, &dir);

	// 慣性移動
	pComputer->phys.move.x += dir.x * MOVEMENT.x;
	pComputer->phys.move.y += dir.y * MOVEMENT.y;
	pComputer->phys.move.z += dir.z * MOVEMENT.z;
}

//=============================================================================
// 最終エサ回収
//=============================================================================
void FinalCollect(Computer* pComputer)
{
	if (pComputer->nTargetPotIdx < 0)
	{// どのタコつぼもターゲットしてない
		return;
	}

	Pot* pPot = GetPot();

	D3DXVECTOR3 target = pPot[pComputer->nTargetPotIdx].pos;
	D3DXVECTOR3 dir = target - pComputer->phys.pos;
	D3DXVec3Normalize(&dir, &dir);

	// 慣性移動
	pComputer->phys.move.x += dir.x * MOVEMENT.x;
	pComputer->phys.move.y += dir.y * MOVEMENT.y;
	pComputer->phys.move.z += dir.z * MOVEMENT.z;

	D3DXVECTOR3 posDiff = pPot->pos - pComputer->phys.pos;
	float dist = D3DXVec3Length(&posDiff);

	if (dist < TENTACLE_RANGE)
	{// 触手で高速移動も積極的に使う
		FindTentacleTarget(pComputer);
		UseTentacle(pComputer);
	}

	if (dist < POT_CLOSE_DISTANCE)
	{// 到達したら即回収
		Pot* pPot = GetPot();
		pPot = &pPot[pComputer->nTargetPotIdx];

		// 全部回収
		pComputer->nFoodCount += pPot->nFood;
		pPot->nFood = 0;

		// 回収後は次のタコつぼへ
		pComputer->state = CPUSTATE_EXPLORE;
	}
}

//=============================================================================
// エサの場所を取得
//=============================================================================
D3DXVECTOR3 GetFoodPosition(Computer* pComputer)
{
	if (pComputer->nTargetFoodIdx < 0)
	{// ターゲットが無い場合は自分の位置を返す
		return pComputer->phys.pos;
	}

	int nIdx = pComputer->nTargetFoodIdx;
	Esa* pEsa = GetEsa();

	// 範囲外チェック・消えていたら無効
	if (nIdx >= MAX_SET_ESA || pEsa[nIdx].bUse == false)
	{// ターゲットをリセット
		pComputer->nTargetFoodIdx = -1;

		return pComputer->phys.pos;
	}

	// エサの座標を返す
	return pEsa[nIdx].pos;
}

//=============================================================================
// 敵の場所を取得
//=============================================================================
D3DXVECTOR3 GetEnemyPosition(Computer* pComputer)
{
	if (pComputer->nTargetEnemyIdx < 0)
	{// ターゲットが無い場合は自分の位置を返す
		return pComputer->phys.pos;
	}

	int nIdx = pComputer->nTargetEnemyIdx;
	Computer* computer = GetComputer();

	// 範囲外チェック
	if (nIdx >= MAX_COMPUTER || computer[nIdx].bUse == false)
	{// ターゲットをリセット
		pComputer->nTargetEnemyIdx = -1;

		return pComputer->phys.pos;
	}

	// エサの座標を返す
	return computer[nIdx].phys.pos;
}

//=============================================================================
// 一番近い敵の場所を取得
//=============================================================================
D3DXVECTOR3 GetNearestEnemy(Computer* pComputer)
{
	float fBestDist = 9999.9f;
	int nBestCount = -1;
	Computer* pEnemy = GetComputer();

	// 最も危険な敵を探す
	for (int nCntEnemy = 0; nCntEnemy < MAX_COMPUTER; nCntEnemy++, pEnemy++)
	{
		if (pEnemy->nIdx == pComputer->nIdx)
		{// 自分自身は無視
			continue;
		}

		D3DXVECTOR3 toEnemy = pEnemy->phys.pos - pComputer->phys.pos;

		float dist = D3DXVec3Length(&toEnemy);

		if (dist > ESCAPE_ENEMY_DIST)
		{// 遠すぎる敵は無視
			continue;
		}

		if (fBestDist > dist)
		{// より近い
			fBestDist = dist;
			nBestCount = nCntEnemy;
		}
	}

	if (nBestCount == -1)
	{// 誰も近くない
		return pComputer->phys.pos;
	}

	return pEnemy[nBestCount].phys.pos;
}

//=============================================================================
// エサのスコア計算
//=============================================================================
void CalcFoodScore(Computer* pComputer)
{
	Esa* pEsa = GetEsa();

	if (pComputer->nTargetFoodIdx == -1)
	{// 何もターゲットしていない
		pComputer->fFoodScore = 0.0f;
		pComputer->nTargetFoodIdx = -1;

		float bestScore = -9999.0f;

		for (int nCntEsa = 0; nCntEsa < MAX_SET_ESA; nCntEsa++, pEsa++)
		{
			if (pEsa->bUse == false)
			{// 使用していないならスキップ
				continue;
			}

			D3DXVECTOR3 foodPos = pEsa->pos;
			D3DXVECTOR3 toFood = foodPos - pComputer->phys.pos;

			float dist = D3DXVec3Length(&toFood);

			if (dist > FAR_DISTANCE)
			{// 遠すぎるエサは無視
				continue;
			}

			float score = 0.0f;

			// 距離が近いほど高スコア
			score += (FAR_DISTANCE - dist) * DISTANCE_SCORE;

			// 進行方向にあるか（dir と toFood の内積）
			D3DXVECTOR3 dirNorm, toFoodNorm;
			D3DXVec3Normalize(&dirNorm, &pComputer->phys.dir);
			D3DXVec3Normalize(&toFoodNorm, &toFood);

			float fDot = D3DXVec3Dot(&dirNorm, &toFoodNorm);

			if (fDot > 0.5f)
			{// 進行方向内
				score += ESA_DOT_SCORE;
			}

			if (IsEnemyNear(pComputer->nIdx, foodPos, ESA_ENEMY_DISTANCE) == false)
			{// 敵が近くにいないか
				score += ESA_ENEMY_DIST_SCORE;
			}

			if (IsBehindPillar(pComputer->phys.pos, foodPos) == true)
			{// 柱の裏側にあるか
				score -= ESA_PILLAR_SCORE;
			}

			if (pEsa->esaType == ESATYPE_LAND)
			{// 止まっている
				score += ESA_WATER_SCORE;
			}

			if (IsNearWall(foodPos) == true)
			{// 壁際にある
				score += ESA_NEAR_WALL_SCORE;
			}

			if (score > bestScore)
			{// 最もスコアが高いエサを選ぶ
				bestScore = score;
				pComputer->nTargetFoodIdx = nCntEsa;
			}
		}

		pComputer->fFoodScore = bestScore;
	}
}

//=============================================================================
// 攻撃のスコア計算
//=============================================================================
void CalcAttackScore(Computer* pComputer)
{
	pComputer->fAttackScore = 0.0f;
	pComputer->nTargetEnemyIdx = -1;

	float bestScore = -9999.0f;

	for (int nCntEnemy = 0; nCntEnemy < MAX_COMPUTER; nCntEnemy++)
	{
		Computer* pEnemy = &g_aComputer[nCntEnemy];

		if (pEnemy->nIdx == pComputer->nIdx)
		{// 自分自身は無視
			continue;
		}

		D3DXVECTOR3 toEnemy = pEnemy->phys.pos - pComputer->phys.pos;
		float dist = D3DXVec3Length(&toEnemy);
		D3DXVec3Normalize(&pComputer->phys.dir, &toEnemy);

		if (dist > FAR_DISTANCE)
		{// 遠すぎる敵は無視
			continue;
		}

		float score = 0.0f;

		// 距離が近いほど高スコア
		score += (FAR_DISTANCE - dist) * DISTANCE_SCORE;

		// 敵がエサを多く持っている
		score += pEnemy->nFoodCount * 0.5f;

		// 敵が自分の進行方向にいるか（内積）
		D3DXVECTOR3 dirNorm, toEnemyNorm;
		D3DXVec3Normalize(&dirNorm, &pComputer->phys.dir);
		D3DXVec3Normalize(&toEnemyNorm, &toEnemy);

		float fDot = D3DXVec3Dot(&dirNorm, &toEnemyNorm);

		if (fDot > 0.5f)
		{ // 慣性で追いやすい
			score += ENEMY_DOT_SCORE;
		}

		if (IsNearWall(pEnemy->phys.pos) == false)
		{// 敵が壁から離れている
			score += ENEMY_FAR_WALL_SCORE;
		}

		if (IsBehindPillar(pComputer->phys.pos, pEnemy->phys.pos) == false)
		{// 敵が柱の裏側にいない
			score += ENEMY_PILLAR_SCORE;
		}

		if (pEnemy->bBlinded)
		{// 敵が墨で視界が悪い
			score += ENEMY_BLIND_SCORE;
		}

		if (pEnemy->nTentacleCooldown > 0)
		{// 敵の触手がクールダウン中
			score += ENEMY_TENTACLE_SCORE;
		}

		if (CountEnemiesNear(pComputer->nIdx, pComputer->phys.pos, ENEMY_COUNT_DIST) >= 2)
		{// 敵が複数近くにいるなら危険
			score -= ENEMY_COUNT_SCORE;
		}

		if (score > bestScore)
		{// 最もスコアが高い敵を選ぶ
			bestScore = score;
			pComputer->nTargetEnemyIdx = nCntEnemy;
		}
	}

	pComputer->fAttackScore = bestScore;
}

//=============================================================================
// 逃げるかのスコア計算
//=============================================================================
void CalcEscapeScore(Computer* pComputer)
{
	pComputer->fEscapeScore = 0.0f;

	float bestScore = -9999.0f;

	// 最も危険な敵を探す
	for (int nCntEnemy = 0; nCntEnemy < MAX_COMPUTER; nCntEnemy++)
	{
		Computer* pEnemy = &g_aComputer[nCntEnemy];

		if (pEnemy->nIdx == pComputer->nIdx)
		{// 自分自身は無視
			continue;
		}

		D3DXVECTOR3 toEnemy = pEnemy->phys.pos - pComputer->phys.pos;

		float dist = D3DXVec3Length(&toEnemy);

		if (dist > ESCAPE_ENEMY_DIST)
		{// 遠すぎる敵は無視
			continue;
		}

		float score = 0.0f;

		// 敵が近いほど逃げるべき
		score += (ESCAPE_ENEMY_DIST - dist) * DISTANCE_SCORE;

		// 敵が後方にいる（内積が負）
		D3DXVECTOR3 dirNorm, toEnemyNorm;
		D3DXVec3Normalize(&dirNorm, &pComputer->phys.dir);
		D3DXVec3Normalize(&toEnemyNorm, &toEnemy);

		float fDot = D3DXVec3Dot(&dirNorm, &toEnemyNorm);

		if (fDot < ESCAPE_DOT)
		{// 後ろから追われている
			score += ESCAPE_DOT_SCORE;
		}

		float mySpeed = D3DXVec3Length(&pComputer->phys.move);
		float enemySpeed = D3DXVec3Length(&pEnemy->phys.move);

		if (enemySpeed > mySpeed)
		{// 敵の速度が自分より速い
			score += ESCAPE_ENEMY_MOVE;
		}

		if (CountEnemiesNear(pComputer->nIdx, pComputer->phys.pos, ENEMY_COUNT_DIST) >= 2)
		{// 敵が複数近くにいる
			score += ESCAPE_ENEMY_SCORE;
		}

		// 自分のエサが多い
		score += pComputer->nFoodCount * ESCAPE_ESA_COUNT;

		if (IsNearPillar(pComputer->phys.pos) == true)
		{// 柱が近い
			score += ESCAPE_PILLAR_SCORE;
		}

		if (pEnemy->nTentacleCooldown == 0)
		{// 敵の触手がクールダウンしていない
			score += ESCAPE_TENTACLE_SCORE;
		}

		if (pEnemy->nInkCooldown < ESCAPE_INK_COUNT)
		{// 敵が墨を吐いた直後
			score += ESCAPE_INK_SCORE;
		}

		if (score > bestScore)
		{// 最も危険な敵のスコアを採用
			bestScore = score;
		}
	}

	pComputer->fEscapeScore = bestScore;
}

//=============================================================================
// 墨吐きのスコア計算
//=============================================================================
void CalcInkScore(Computer* pComputer)
{
	pComputer->fInkScore = 0.0f;

	if (pComputer->nInkCooldown > 0)
	{// クールダウン中は絶対に使えない
		pComputer->fInkScore = -9999.0f;

		return;
	}

	float bestScore = -9999.0f;

	for (int nCntEnemy = 0; nCntEnemy < MAX_COMPUTER; nCntEnemy++)
	{
		Computer* pEnemy = &g_aComputer[nCntEnemy];

		if (pEnemy->nIdx == pComputer->nIdx)
		{// 自分自身は無視
			continue;
		}

		D3DXVECTOR3 toEnemy = pEnemy->phys.pos - pComputer->phys.pos;

		float dist = D3DXVec3Length(&toEnemy);

		if (dist > FAR_DISTANCE)
		{// 遠すぎる敵は無視
			continue;
		}

		float score = 0.0f;

		// 敵が近いほど墨が有効
		score += (FAR_DISTANCE - dist) * DISTANCE_SCORE;

		// 敵が後方にいる
		D3DXVECTOR3 dirNorm, toEnemyNorm;
		D3DXVec3Normalize(&dirNorm, &pComputer->phys.dir);
		D3DXVec3Normalize(&toEnemyNorm, &toEnemy);

		float fDot = D3DXVec3Dot(&dirNorm, &toEnemyNorm);

		if (fDot < ESCAPE_DOT)
		{// 後ろから追われている
			score += ESCAPE_DOT_SCORE;
		}

		// 敵が自分より速い（追いつかれる）
		float mySpeed = D3DXVec3Length(&pComputer->phys.move);
		float enemySpeed = D3DXVec3Length(&pEnemy->phys.move);

		if (enemySpeed > mySpeed)
		{// 敵の速度が自分より速い
			score += INK_ENEMY_MOVE;
		}

		// 敵がエサを多く持っている
		score += pEnemy->nFoodCount * INK_ENEMY_ESA_COUNT;

		if (CountEnemiesNear(pComputer->nIdx, pComputer->phys.pos, ENEMY_COUNT_DIST) >= 2)
		{// 敵が複数近くにいる
			score += INK_ENEMY_COUNT_SCORE;
		}

		// 自分のエサが多い
		score += pComputer->nFoodCount * INK_ESA_COUNT;

		if (IsNearPillar(pComputer->phys.pos) == true)
		{// 柱が近い
			score += INK_PILLAR_SCORE;
		}

		if (score > bestScore)
		{// 最も墨が有効な敵を採用
			bestScore = score;
		}
	}

	pComputer->fInkScore = bestScore;
}

//=============================================================================
// タコつぼ関係のスコア計算
//=============================================================================
void CalcPotScore(Computer* pComputer)
{
	pComputer->fPotScore = 0.0f;
	pComputer->nTargetPotIdx = -1;

	float bestScore = -9999.0f;

	for (int nCntPot = 0; nCntPot < MAX_POT; nCntPot++)
	{
		Pot* pPot = GetPot();
		pPot = &pPot[nCntPot];

		D3DXVECTOR3 toPot = pPot->pos - pComputer->phys.pos;

		float dist = D3DXVec3Length(&toPot);

		if (dist > POT_DISTANCE || dist < POT_CLOSE_DISTANCE * 0.1f || pPot->bUse == false)
		{// 遠い/近いタコつぼは無視
			continue;
		}

		float score = 0.0f;

		// 自分のエサを隠したい
		score += pComputer->nFoodCount * POT_ESA_SCORE;

		// タコつぼが近い
		score += (POT_DISTANCE - dist) * DISTANCE_SCORE;

		if (IsEnemyNear(pComputer->nIdx, pPot->pos, POT_ENEMY_DISTANCE) == true)
		{// 敵が近くにいない
			score += POT_ENEMY_DIST_SCORE;
		}

		// 終盤ボーナス
		if (GetTime() < POT_PHASE_1)
		{
			score += POT_PHASE_1_SCORE;
		}

		if (GetTime() < POT_PHASE_2)
		{
			score += POT_PHASE_2_SCORE;
		}

		if (score > bestScore)
		{// 最も有効なタコつぼを採用
			bestScore = score;
			pComputer->nTargetPotIdx = nCntPot;
		}
	}

	pComputer->fPotScore = bestScore;
}

//=============================================================================
// 敵が近くにいるか
//=============================================================================
bool IsEnemyNear(int nIdx, D3DXVECTOR3 pos, float fRange)
{
	for (int nCntEnemy = 0; nCntEnemy < MAX_COMPUTER; nCntEnemy++)
	{
		if (nCntEnemy == nIdx)
		{// 自分とは判定しない
			continue;
		}

		D3DXVECTOR3 posDiff = g_aComputer[nCntEnemy].phys.pos - pos;

		float dist = D3DXVec3Length(&posDiff);

		if (dist < fRange)
		{// 近い
			return true;
		}
	}

	return false;
}

//=============================================================================
// 柱の裏側か
//=============================================================================
bool IsBehindPillar(D3DXVECTOR3 self, D3DXVECTOR3 target)
{
	// 中央柱の中心
	D3DXVECTOR3 pillarCenter = D3DXVECTOR3(0, self.y, 0);

	D3DXVECTOR3 toSelf = self - pillarCenter;
	D3DXVECTOR3 toTarget = target - pillarCenter;

	// 角度が大きいほど裏側
	float dot = D3DXVec3Dot(&toSelf, &toTarget);

	// 反対側
	return (dot < 0.0f);
}

//=============================================================================
// 壁際か
//=============================================================================
bool IsNearWall(D3DXVECTOR3 pos)
{
	float r = sqrtf(pos.x * pos.x + pos.z * pos.z);

	if (fabs(r - OUTCYLINDER_RADIUS) < NEAR_WALL_DISTANCE)
	{// 外周
		return true;
	}

	if (fabs(r - INCYLINDER_RADIUS) < NEAR_WALL_DISTANCE)
	{// 中央柱
		return true;
	}

	return false;
}

//=============================================================================
// 敵が複数近くにいるか
//=============================================================================
int CountEnemiesNear(int nIdx, D3DXVECTOR3 pos, float range)
{
	int nCount = 0;

	for (int nCntEnemy = 0; nCntEnemy < MAX_COMPUTER; nCntEnemy++)
	{
		if (nCntEnemy == nIdx)
		{// 自分とは判定しない
			continue;
		}

		D3DXVECTOR3 posDiff = g_aComputer[nCntEnemy].phys.pos - pos;

		float dist = D3DXVec3Length(&posDiff);

		if (dist < range)
		{// 近い
			nCount++;
		}
	}

	return nCount;
}

//=============================================================================
// 柱が近いか
//=============================================================================
bool IsNearPillar(D3DXVECTOR3 pos)
{
	float r = sqrtf(pos.x * pos.x + pos.z * pos.z);

	return (fabs(r - INCYLINDER_RADIUS) < NEAR_PILLAR_DISTANCE);
}

//=============================================================================
// 触手移動のレイキャスト
//=============================================================================
void FindTentacleTarget(Computer* pComputer)
{
	float bestScore = -99999.0f;
	D3DXVECTOR3 bestPoint(0, 0, 0);

	// 360度にレイを飛ばす（例：16方向）
	for (int nCntRay = 0; nCntRay < RAY_COUNT; nCntRay++)
	{
		float angle = (D3DX_PI * 2 / RAY_COUNT) * nCntRay;

		D3DXVECTOR3 dir(cosf(angle), 0, sinf(angle));

		// 外周と中央柱の両方にレイキャスト
		float distOuter, distInner;
		bool hitOuter = RaycastToOuterWall(pComputer->phys.pos, dir, &distOuter);
		bool hitInner = RaycastToInnerPillar(pComputer->phys.pos, dir, &distInner);

		if (hitOuter == false && hitInner == false)
		{// どちらにも当たらない方向は無視
			continue;
		}

		// 近い方の壁を採用
		float dist = 999999.0f;

		if (hitOuter == true)
		{// 外円
			dist = distOuter;
		}

		if (hitInner == true && distInner < dist)
		{// 内円
			dist = distInner;
		}

		// ヒット位置
		D3DXVECTOR3 hitPoint = pComputer->phys.pos + dir * dist;

		// スコア計算：自分の向いている方向に近いほど高い
		D3DXVECTOR3 dirNorm;
		D3DXVec3Normalize(&dirNorm, &pComputer->phys.dir);

		float score = D3DXVec3Dot(&dirNorm, &dir);

		if (dist < 1500.0f)
		{// 距離が近すぎると触手が短くなるので少し減点
			score -= 1.0f;
		}

		if (score > bestScore)
		{// 最良の方向を採用
			bestScore = score;
			bestPoint = hitPoint;
		}
	}

	pComputer->targetWall = bestPoint;
}

//=============================================================================
// レイキャストと外円との当たり判定
//=============================================================================
bool RaycastToOuterWall(D3DXVECTOR3 origin, D3DXVECTOR3 dir, float* hitDist)
{
	// 円柱の半径
	float R = OUTCYLINDER_RADIUS;

	// レイのXY成分だけ使う
	float ox = origin.x;
	float oz = origin.z;
	float dx = dir.x;
	float dz = dir.z;

	// 2次方程式の係数
	float a = dx * dx + dz * dz;
	float b = 2 * (ox * dx + oz * dz);
	float c = ox * ox + oz * oz - R * R;

	float D = b * b - 4 * a * c;

	if (D < 0)
	{// 当たらない 
		return false;
	}

	float t = (-b - sqrtf(D)) / (2 * a);

	if (t < 0)
	{// 後ろ側
		return false;
	}

	*hitDist = t;

	return true;
}

//=============================================================================
// レイキャストと内円との当たり判定
//=============================================================================
bool RaycastToInnerPillar(D3DXVECTOR3 origin, D3DXVECTOR3 dir, float* hitDist)
{
	// 中央柱の半径（例）
	float R = INCYLINDER_RADIUS;

	// レイのXZ成分だけ使う
	float ox = origin.x;
	float oz = origin.z;
	float dx = dir.x;
	float dz = dir.z;

	// 2次方程式の係数
	float a = dx * dx + dz * dz;
	float b = 2 * (ox * dx + oz * dz);
	float c = ox * ox + oz * oz - R * R;

	float D = b * b - 4 * a * c;

	if (D < 0)
	{// 当たらない
		return false;
	}

	float t = (-b - sqrtf(D)) / (2 * a);

	if (t < 0)
	{// 後ろ側
		return false;
	}

	*hitDist = t;

	return true;
}

//=============================================================================
// 触手移動を使うべきかどうか
//=============================================================================
bool ShouldUseTentacle(Computer* pComputer)
{
	if (pComputer->nTentacleCooldown > 0)
	{// クールダウン中は使えない
		return false;
	}

	if (D3DXVec3Length(&pComputer->phys.move) < 20.0f)
	{// 速度が遅いときは使う（加速目的）
		return true;
	}

	if (pComputer->state == CPUSTATE_ESCAPE)
	{// 敵から逃げているとき
		return true;
	}

	if (pComputer->state == CPUSTATE_GO_TO_POT)
	{// タコつぼが遠いとき
		return true;
	}

	return false;
}

//=============================================================================
// 触手移動
//=============================================================================
void UseTentacle(Computer* pComputer)
{
	// ターゲットが無い場合は何もしない
	D3DXVECTOR3 target = pComputer->targetWall;
	D3DXVECTOR3 toWall = target - pComputer->phys.pos;

	float dist = D3DXVec3Length(&toWall);

	if (dist < 1.0f)
	{// 近すぎる
		return;
	}

	// 方向ベクトルを正規化
	D3DXVECTOR3 dir;
	D3DXVec3Normalize(&dir, &toWall);

	// 触手による強制加速
	pComputer->phys.move.x += dir.x * TENTACLE_MOVEMENT.x;
	pComputer->phys.move.y += dir.y * TENTACLE_MOVEMENT.y;
	pComputer->phys.move.z += dir.z * TENTACLE_MOVEMENT.z;

	// キャラの向きを触手方向へ合わせる
	pComputer->phys.dir = dir;

	// 触手クールダウン
	pComputer->nTentacleCooldown = TENTACLE_COOLDOWN;

	// 触手モーション開始
}

//=============================================================================
// ノード設置(外周)
//=============================================================================
void CreateOuterNodes3D(void)
{
	int ringCount = NODE_COUNT;          // 1周のノード数
	float radius = OUTCYLINDER_RADIUS;

	float heights[NODE_HEIGHT] = { CYLINDER_HEIGHT, CYLINDER_HEIGHT * 0.5f, 0.0f }; // 上・中・下

	int index = 0;

	for (int nCntHeight = 0; nCntHeight < NODE_HEIGHT; nCntHeight++)
	{
		for (int nCntNode = 0; nCntNode < ringCount; nCntNode++)
		{
			float angle = (D3DX_PI * 2 / ringCount) * nCntNode;

			g_aOutNode[index].pos.x = cosf(angle) * radius;
			g_aOutNode[index].pos.y = heights[nCntHeight];
			g_aOutNode[index].pos.z = sinf(angle) * radius;

			g_aOutNode[index].bUse = true;

			index++;
		}
	}
}

//=============================================================================
// ノード設置(内周)
//=============================================================================
void CreateInnerNodes3D(void)
{
	int ringCount = NODE_COUNT;          // 1周のノード数
	float radius = INCYLINDER_RADIUS;

	float heights[NODE_HEIGHT] = { CYLINDER_HEIGHT, CYLINDER_HEIGHT * 0.5f, 0.0f }; // 上・中・下

	int index = 0;

	for (int nCntHeight = 0; nCntHeight < NODE_HEIGHT; nCntHeight++)
	{
		for (int nCntNode = 0; nCntNode < ringCount; nCntNode++)
		{
			float angle = (D3DX_PI * 2 / ringCount) * nCntNode;

			g_aInNode[index].pos.x = cosf(angle) * radius;
			g_aInNode[index].pos.y = heights[nCntHeight];
			g_aInNode[index].pos.z = sinf(angle) * radius;

			g_aInNode[index].bUse = true;

			index++;
		}
	}
}

//=============================================================================
// ランダムなノードを取得
//=============================================================================
D3DXVECTOR3 GetRandomExplorePoint(void)
{
	// 外周ノード + 内周ノードの総数
	int totalNodes = NODE_COUNT * NODE_HEIGHT * 2;

	// ランダムにノードを選ぶ
	int nIdx = rand() % totalNodes;

	if (nIdx < NODE_COUNT * NODE_HEIGHT)
	{// 外周ノード
		return g_aOutNode[nIdx].pos;
	}
	else
	{// 内周ノード
		return g_aInNode[nIdx - (NODE_COUNT * NODE_HEIGHT)].pos;
	}
}

//=============================================================================
// CPUの設定処理
//=============================================================================
void SetComputer(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	Computer* pComputer = GetComputer();

	for (int nCntComputer = 0; nCntComputer < MAX_COMPUTER; nCntComputer++, pComputer++)
	{
		if (pComputer->bUse == false)
		{// 使用していない
			pComputer->state = CPUSTATE_APPEAR;
			pComputer->nCounterState = 0;
			pComputer->phys.pos = pos;
			pComputer->phys.posOld = pos;
			pComputer->phys.move = FIRST_POS;
			pComputer->phys.rot = rot;
			pComputer->phys.dir = FIRST_POS;
			pComputer->phys.fAngle = 0.0f;
			pComputer->phys.fRadius = CPU_WIDTH;
			pComputer->phys.fHeight = CPU_HEIGHT;
			pComputer->bUse = true;

			pComputer->nTargetFoodIdx = -1;
			pComputer->nTargetEnemyIdx = -1;
			pComputer->nTargetPotIdx = -1;
			pComputer->targetWall = FIRST_POS;
			pComputer->extarget = pos;
			pComputer->posX = FIRST_POS;
			pComputer->vecX = FIRST_POS;

			pComputer->fFoodScore = 0;
			pComputer->fAttackScore = 0;
			pComputer->fEscapeScore = 0;
			pComputer->fInkScore = 0;

			pComputer->nTentacleCooldown = 0;
			pComputer->nInkCooldown = 0;
			pComputer->nThinkCooldown = 0;

			pComputer->bBlinded = false;
			pComputer->nBlindCounter = 0;

			pComputer->nFoodCount = 0;
			pComputer->nMaxFood = 1;

			pComputer->nCurrentNode = 0;
			pComputer->nNextNode = 0;

			pComputer->motionType = MOTIONTYPE_NEUTRAL;
			pComputer->bLoopMotion = pComputer->aMotionInfo[MOTIONTYPE_NEUTRAL].bLoop;
			pComputer->nNumKey = pComputer->aMotionInfo[MOTIONTYPE_NEUTRAL].nNumKey;
			pComputer->nKey = 0;
			pComputer->nCounterMotion = 0;
			pComputer->bFinishMotion = false;
			pComputer->bBlendMotion = false;
			pComputer->motionTypeBlend = MOTIONTYPE_NEUTRAL;
			pComputer->bLoopMotionBlend = pComputer->aMotionInfo[MOTIONTYPE_NEUTRAL].bLoop;
			pComputer->nNumKeyBlend = pComputer->aMotionInfo[MOTIONTYPE_NEUTRAL].nNumKey;
			pComputer->nKeyBlend = 0;
			pComputer->nCounterMotionBlend = 0;
			pComputer->nFrameBlend = 0;
			pComputer->nCounterBlend = 0;

			break;
		}
	}
}

//=============================================================================
// CPUのランダム設定処理
//=============================================================================
void SetRandomComputer(int nAmount)
{
	for (int nCntCPU = 0; nCntCPU < nAmount; nCntCPU++)
	{
		D3DXVECTOR3 pos;
		float fAngle = (D3DX_PI * 2.0f) * ((float)((nCntCPU + 1) * (360.0f / nAmount)) / 360.0f);
		float fsin = sinf(fAngle);

		pos.x = sinf(fAngle) * (INCYLINDER_RADIUS + (((float)(rand() % (int)(OUTCYLINDER_RADIUS - INCYLINDER_RADIUS) + 1))));
		pos.y = (float)(rand() % (int)(CYLINDER_HEIGHT * 0.6f)) + (CYLINDER_HEIGHT * 0.2f);
		pos.z = cosf(fAngle) * (INCYLINDER_RADIUS + (((float)(rand() % (int)(OUTCYLINDER_RADIUS - INCYLINDER_RADIUS) + 1))));

		SetComputer(pos, FIRST_POS);
	}
}

//=============================================================================
// CPUの取得処理
//=============================================================================
Computer* GetComputer(void)
{
	return &g_aComputer[0];
}

//=============================================================================
// CPUのロード処理
//=============================================================================
void LoadComputer(void)
{
	// ローカル変数宣言
	FILE* pFile;
	Computer* pComputer = GetComputer();
	char aString[512] = {};				// ファイルのテキスト読み込み
	char aTrash[512] = {};				// ごみ箱
	char aModelName[128][512] = {};		// モデルの名前

	// テクスチャ読み込み用の変数
	int nNumTexture = 0;
	char aTextureName[64][256] = {};		// テクスチャの名前

	// キャラクターセット用の変数
	int nNumParts = 0;		// 読み込むパーツ数
	float fRadius = 0.0f;	// キャラクターの半径
	float fHeight = 0.0f;	// キャラクターの高さ
	float fMove = 0.0f;		// キャラクターの移動量
	float fJump = 0.0f;		// キャラクターのジャンプ量
	D3DXVECTOR2 Blowoff = D3DXVECTOR2(0.0f, 0.0f);		// 吹っ飛び量
	D3DXVECTOR2 Somersault = D3DXVECTOR2(0.0f, 0.0f);	// バク宙量

	// パーツセット用の変数
	int nIdx = -1;			// モデル番号
	int nIdxParent = -1;	// 親のモデル番号
	D3DXVECTOR3 pos = FIRST_POS;		// 位置
	D3DXVECTOR3 rot = FIRST_POS;		// 向き

	// モーションセット用の変数
	int nLoop = 0;			// ループするかどうか
	int nNumKey = 0;		// キーの総数
	int nFrame = 0;			// 再生フレーム数
	int nCntMotion = 0;		// モーション番号
	KEY key = {};			// キー要素

	pFile = fopen(CPU_FILE, "r");

	if (pFile != NULL)
	{// ファイルが開けた場合
		fscanf(pFile, "%[^SCRIPT]", &aTrash[0]);

		while (1)
		{
			fscanf(pFile, "%s", &aString[0]);

			if (aString[0] == '#')
			{// コメントは無視
				fgets(&aTrash[0], ONE_LINE, pFile);

				continue;
			}

			if (strcmp(&aString[0], "NUM_MODEL") == 0)
			{// モデル数の読み込み
				fscanf(pFile, " = %d", &pComputer->nNumModel);

				continue;
			}

			if (strcmp(&aString[0], "MODEL_FILENAME") == 0)
			{// モデルの名前読み込み
				for (int nCntModel = 0; nCntModel < pComputer->nNumModel; nCntModel++)
				{
					if (nCntModel > 0)
					{// 2回目以降のMODEL_FILENAMEを読み込んでおく
						if (strcmp(&aString[0], "MODEL_FILENAME") != 0)
						{// コメントの代わりに読み込んだ場合はなし
							fscanf(pFile, "%s", &aTrash[0]);
						}
					}

					fscanf(pFile, " = %s", &aModelName[nCntModel][0]);		// モデルのパス
					g_apFilenameComputer[nCntModel] = &aModelName[nCntModel][0];

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);
					}
				}

				continue;
			}

			if (strcmp(&aString[0], "NUM_TEXTURE") == 0)
			{// テクスチャ数の読み込み
				fscanf(pFile, " = %d", &nNumTexture);

				continue;
			}

			if (strcmp(&aString[0], "TEXTURE_FILENAME") == 0)
			{// テクスチャの名前読み込み
				for (int nCntTexture = 0; nCntTexture < nNumTexture; nCntTexture++)
				{
					if (nCntTexture > 0)
					{// 2回目以降のTEXTURE_FILENAMEを読み込んでおく
						if (strcmp(&aString[0], "TEXTURE_FILENAME") != 0)
						{// コメントの代わりに読み込んだ場合はなし
							fscanf(pFile, "%s", &aTrash[0]);
						}
					}

					fscanf(pFile, " = %s", &aTextureName[nCntTexture][0]);		// テクスチャのパス

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);
					}
				}

				continue;
			}

			if (strcmp(&aString[0], "CHARACTERSET") == 0)
			{// キャラクターの設定
				fscanf(pFile, "%s", &aString[0]);

				if (strcmp(&aString[0], "NUM_PARTS") == 0)
				{// 読み込むパーツ数
					fscanf(pFile, " = %d", &nNumParts);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "RADIUS") == 0)
				{// キャラクターの半径
					fscanf(pFile, " = %f", &pComputer->phys.fRadius);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "HEIGHT") == 0)
				{// キャラクターの高さ
					fscanf(pFile, " = %f", &pComputer->phys.fHeight);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "MOVE") == 0)
				{// キャラクターの移動量
					fscanf(pFile, " = %f", &fMove);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "JUMP") == 0)
				{// キャラクターのジャンプ量
					fscanf(pFile, " = %f", &fJump);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "BLOWOFF") == 0)
				{// キャラクターの吹っ飛び量
					fscanf(pFile, " = %f %f", &Blowoff.x, &Blowoff.y);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "SOMERSAULT") == 0)
				{// キャラクターのバク宙量
					fscanf(pFile, " = %f %f", &Somersault.x, &Somersault.y);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				// パーツ読み込み
				for (int nCntParts = 0; nCntParts < nNumParts; nCntParts++)
				{
					if (strcmp(&aString[0], "PARTSSET") == 0)
					{// PARTSSET
						fscanf(pFile, "%s", &aString[0]);
					}

					if (strcmp(&aString[0], "INDEX") == 0)
					{// モデル番号
						fscanf(pFile, " = %d", &nIdx);

						fscanf(pFile, "%s", &aString[0]);

						if (aString[0] == '#')
						{// コメント無視
							fgets(&aTrash[0], ONE_LINE, pFile);

							// 次の文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);
						}
					}

					if (strcmp(&aString[0], "PARENT") == 0)
					{// 親のモデル番号
						fscanf(pFile, " = %d", &nIdxParent);

						fscanf(pFile, "%s", &aString[0]);

						if (aString[0] == '#')
						{// コメント無視
							fgets(&aTrash[0], ONE_LINE, pFile);

							// 次の文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);
						}
					}

					if (strcmp(&aString[0], "POS") == 0)
					{// パーツの位置(オフセット)
						fscanf(pFile, " = %f %f %f", &pos.x, &pos.y, &pos.z);

						fscanf(pFile, "%s", &aString[0]);

						if (aString[0] == '#')
						{// コメント無視
							fgets(&aTrash[0], ONE_LINE, pFile);

							// 次の文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);
						}
					}

					if (strcmp(&aString[0], "ROT") == 0)
					{// パーツの向き
						fscanf(pFile, " = %f %f %f", &rot.x, &rot.y, &rot.z);

						fscanf(pFile, "%s", &aString[0]);

						if (aString[0] == '#')
						{// コメント無視
							fgets(&aTrash[0], ONE_LINE, pFile);

							// 次の文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);
						}
					}

					if (strcmp(&aString[0], "END_PARTSSET") == 0)
					{// END_PARTSSET
						fscanf(pFile, "%s", &aString[0]);
					}

					// プレイヤーのモデル情報に設定
					pComputer->aModel[nCntParts].nIdx = nIdx;
					pComputer->aModel[nCntParts].nIdxModelParent = nIdxParent;
					pComputer->aModel[nCntParts].pos = pos;
					pComputer->aModel[nCntParts].rot = rot;
					pComputer->aModel[nCntParts].posOff = pos;
					pComputer->aModel[nCntParts].rotOff = rot;
				}

				continue;
			}

			if (strcmp(&aString[0], "MOTIONSET") == 0)
			{// モーションの設定
				fscanf(pFile, "%s", &aString[0]);

				if (aString[0] == '#')
				{// コメント無視
					fgets(&aTrash[0], ONE_LINE, pFile);

					// 次の文字列を読み込む
					fscanf(pFile, "%s", &aString[0]);
				}

				if (strcmp(&aString[0], "LOOP") == 0)
				{// ループするかどうか
					fscanf(pFile, " = %d", &nLoop);

					// ループの真偽を代入
					if (nLoop == 0)
					{// false
						pComputer->aMotionInfo[nCntMotion].bLoop = false;
					}
					else if (nLoop == 1)
					{// true
						pComputer->aMotionInfo[nCntMotion].bLoop = true;
					}

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "NUM_KEY") == 0)
				{// キーの総数
					fscanf(pFile, " = %d", &nNumKey);

					// キーの総数を代入
					pComputer->aMotionInfo[nCntMotion].nNumKey = nNumKey;

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				// キー読み込み
				for (int nCntKey = 0; nCntKey < nNumKey; nCntKey++)
				{
					if (strcmp(&aString[0], "KEYSET") == 0)
					{// KEYSET
						fscanf(pFile, "%s", &aString[0]);

						if (aString[0] == '#')
						{// コメント無視
							fgets(&aTrash[0], ONE_LINE, pFile);

							// 次の文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);
						}
					}

					if (strcmp(&aString[0], "FRAME") == 0)
					{// 再生フレーム数
						fscanf(pFile, " = %d", &nFrame);

						pComputer->aMotionInfo[nCntMotion].aKeyInfo[nCntKey].nFrame = nFrame;

						fscanf(pFile, "%s", &aString[0]);

						if (aString[0] == '#')
						{// コメント無視
							fgets(&aTrash[0], ONE_LINE, pFile);

							// 次の文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);
						}
					}

					// パーツごとに読み込み
					for (int nCntPartsKey = 0; nCntPartsKey < nNumParts; nCntPartsKey++)
					{
						if (strcmp(&aString[0], "KEY") == 0)
						{// KEY
							fscanf(pFile, "%s", &aString[0]);

							if (aString[0] == '#')
							{// コメント無視
								fgets(&aTrash[0], ONE_LINE, pFile);

								// 次の文字列を読み込む
								fscanf(pFile, "%s", &aString[0]);
							}
						}

						if (strcmp(&aString[0], "POS") == 0)
						{// パーツの位置(オフセット)
							fscanf(pFile, " = %f %f %f", &key.fPosX, &key.fPosY, &key.fPosZ);

							fscanf(pFile, "%s", &aString[0]);

							if (aString[0] == '#')
							{// コメント無視
								fgets(&aTrash[0], ONE_LINE, pFile);

								// 次の文字列を読み込む
								fscanf(pFile, "%s", &aString[0]);
							}
						}

						if (strcmp(&aString[0], "ROT") == 0)
						{// パーツの向き
							fscanf(pFile, " = %f %f %f", &key.fRotX, &key.fRotY, &key.fRotZ);

							fscanf(pFile, "%s", &aString[0]);

							if (aString[0] == '#')
							{// コメント無視
								fgets(&aTrash[0], ONE_LINE, pFile);

								// 次の文字列を読み込む
								fscanf(pFile, "%s", &aString[0]);
							}
						}

						if (strcmp(&aString[0], "END_KEY") == 0)
						{// END_KEY
							fscanf(pFile, "%s", &aString[0]);
						}

						// モーション情報に代入
						pComputer->aMotionInfo[nCntMotion].aKeyInfo[nCntKey].aKey[nCntPartsKey] = key;
					}

					if (strcmp(&aString[0], "END_KEYSET") == 0)
					{// END_KEYSET
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				nCntMotion++;

				continue;
			}

			if (strcmp(&aString[0], "END_SCRIPT") == 0)
			{// ファイルの読み込みが完了
				break;
			}
		}

		// モーションの総数を代入
		pComputer->nNumMotion = nCntMotion + 1;

		for (int nCntCPU = 0; nCntCPU < MAX_COMPUTER - 1; nCntCPU++)
		{
			pComputer[nCntCPU + 1] = pComputer[nCntCPU];
		}

		fclose(pFile);
	}
	else
	{// ファイルが開けなかった場合
		printf("ファイルを開けませんでした");
	}
}

//=============================================================================
// CPUモーションの更新処理
//=============================================================================
void UpdateMotionComputer(void)
{
	int nNextKey;
	Computer* pComputer = GetComputer();

	for (int nCntComputer = 0; nCntComputer < MAX_COMPUTER; nCntComputer++, pComputer++)
	{
		// 全モデル(パーツ)の更新
		for (int nCntModel = 0; nCntModel < pComputer->nNumModel; nCntModel++)
		{
			// ローカル変数宣言
			KEY* pKey;
			KEY* pKeyNext;
			Model* pModel;
			D3DXVECTOR3 posCurrent, rotCurrent;
			D3DXVECTOR3 posDest, rotDest;
			float fDiffKey, fRateKey;

			// 1フレームあたりに動く割合
			fRateKey = ((float)pComputer->nCounterMotion / (float)pComputer->aMotionInfo[pComputer->motionType].aKeyInfo[pComputer->nKey].nFrame);

			// 次のキー
			if (pComputer->bLoopMotion == true)
			{// ループモーション
				nNextKey = (pComputer->nKey + 1) % (pComputer->aMotionInfo[pComputer->motionType].nNumKey);
			}
			else
			{// ループしないモーション
				nNextKey = pComputer->nKey + 1;

				if (nNextKey > pComputer->aMotionInfo[pComputer->motionType].nNumKey - 1)
				{// キーの総数を超えた
					nNextKey = pComputer->aMotionInfo[pComputer->motionType].nNumKey - 1;
				}
			}

			// 現在のキーのポインタ
			pKey = &pComputer->aMotionInfo[pComputer->motionType].aKeyInfo[pComputer->nKey].aKey[nCntModel];

			// 次のキーのポインタ
			pKeyNext = &pComputer->aMotionInfo[pComputer->motionType].aKeyInfo[nNextKey].aKey[nCntModel];

			// モデルのポインタ
			pModel = &pComputer->aModel[nCntModel];

			if (pComputer->bBlendMotion == true)
			{// ブレンドあり
				// ローカル変数宣言
				KEY* pKeyBlend;
				KEY* pKeyNextBlend;
				D3DXVECTOR3 posBlend, rotBlend;
				float fDiffKeyBlend, fDiffBlend, fRateKeyBlend, fRateBlend;
				int nNextKeyBlend;

				// 1フレームあたりに動く割合
				fRateKeyBlend = ((float)pComputer->nCounterMotionBlend / (float)pComputer->aMotionInfo[pComputer->motionTypeBlend].aKeyInfo[pComputer->nKeyBlend].nFrame);

				// ブレンドの相対値
				fRateBlend = (float)pComputer->nCounterBlend / (float)pComputer->nFrameBlend;

				// 次のキー
				if (pComputer->bLoopMotionBlend == true)
				{// ループモーション
					nNextKeyBlend = (pComputer->nKeyBlend + 1) % (pComputer->aMotionInfo[pComputer->motionTypeBlend].nNumKey);
				}
				else
				{// ループしないモーション
					nNextKeyBlend = pComputer->nKeyBlend + 1;

					if (nNextKeyBlend > pComputer->aMotionInfo[pComputer->motionTypeBlend].nNumKey - 1)
					{// キーの総数を超えた
						nNextKeyBlend = pComputer->aMotionInfo[pComputer->motionTypeBlend].nNumKey - 1;
					}
				}

				// 現在のキーのポインタ
				pKeyBlend = &pComputer->aMotionInfo[pComputer->motionTypeBlend].aKeyInfo[pComputer->nKeyBlend].aKey[nCntModel];

				// 次のキーのポインタ
				pKeyNextBlend = &pComputer->aMotionInfo[pComputer->motionTypeBlend].aKeyInfo[nNextKeyBlend].aKey[nCntModel];

				// キー情報から位置・向きを算出
				// 位置X
				fDiffKey = pKeyNext->fPosX - pKey->fPosX;					// 現在のモーション
				posCurrent.x = pKey->fPosX + (fDiffKey * fRateKey);
				fDiffKeyBlend = pKeyNextBlend->fPosX - pKeyBlend->fPosX;	// ブレンドモーション
				posBlend.x = pKeyBlend->fPosX + (fDiffKeyBlend * fRateKeyBlend);
				fDiffBlend = posBlend.x - posCurrent.x;						// 差分
				posDest.x = posCurrent.x + (fDiffBlend * fRateBlend);		// 求める値

				// 位置Y
				fDiffKey = pKeyNext->fPosY - pKey->fPosY;					// 現在のモーション
				posCurrent.y = pKey->fPosY + (fDiffKey * fRateKey);
				fDiffKeyBlend = pKeyNextBlend->fPosY - pKeyBlend->fPosY;	// ブレンドモーション
				posBlend.y = pKeyBlend->fPosY + (fDiffKeyBlend * fRateKeyBlend);
				fDiffBlend = posBlend.y - posCurrent.y;						// 差分
				posDest.y = posCurrent.y + (fDiffBlend * fRateBlend);		// 求める値

				// 位置Z
				fDiffKey = pKeyNext->fPosZ - pKey->fPosZ;					// 現在のモーション
				posCurrent.z = pKey->fPosZ + (fDiffKey * fRateKey);
				fDiffKeyBlend = pKeyNextBlend->fPosZ - pKeyBlend->fPosZ;	// ブレンドモーション
				posBlend.z = pKeyBlend->fPosZ + (fDiffKeyBlend * fRateKeyBlend);
				fDiffBlend = posBlend.z - posCurrent.z;						// 差分
				posDest.z = posCurrent.z + (fDiffBlend * fRateBlend);		// 求める値

				// 向きX
				fDiffKey = pKeyNext->fRotX - pKey->fRotX;					// 現在のモーション
				rotCurrent.x = pKey->fRotX + (fDiffKey * fRateKey);
				fDiffKeyBlend = pKeyNextBlend->fRotX - pKeyBlend->fRotX;	// ブレンドモーション
				rotBlend.x = pKeyBlend->fRotX + (fDiffKeyBlend * fRateKeyBlend);
				fDiffBlend = rotBlend.x - rotCurrent.x;						// 差分
				rotDest.x = rotCurrent.x + (fDiffBlend * fRateBlend);		// 求める値

				// 向きを調整
				if (rotDest.x > D3DX_PI)
				{
					rotDest.x -= D3DX_PI * 2;
				}
				else if (rotDest.x < -D3DX_PI)
				{
					rotDest.x += D3DX_PI * 2;
				}

				// 向きY
				fDiffKey = pKeyNext->fRotY - pKey->fRotY;					// 現在のモーション
				rotCurrent.y = pKey->fRotY + (fDiffKey * fRateKey);
				fDiffKeyBlend = pKeyNextBlend->fRotY - pKeyBlend->fRotY;	// ブレンドモーション
				rotBlend.y = pKeyBlend->fRotY + (fDiffKeyBlend * fRateKeyBlend);
				fDiffBlend = rotBlend.y - rotCurrent.y;						// 差分
				rotDest.y = rotCurrent.y + (fDiffBlend * fRateBlend);		// 求める値

				// 向きを調整
				if (rotDest.y > D3DX_PI)
				{
					rotDest.y -= D3DX_PI * 2;
				}
				else if (rotDest.y < -D3DX_PI)
				{
					rotDest.y += D3DX_PI * 2;
				}

				// 向きZ
				fDiffKey = pKeyNext->fRotZ - pKey->fRotZ;					// 現在のモーション
				rotCurrent.z = pKey->fRotZ + (fDiffKey * fRateKey);
				fDiffKeyBlend = pKeyNextBlend->fRotZ - pKeyBlend->fRotZ;	// ブレンドモーション
				rotBlend.z = pKeyBlend->fRotZ + (fDiffKeyBlend * fRateKeyBlend);
				fDiffBlend = rotBlend.z - rotCurrent.z;						// 差分
				rotDest.z = rotCurrent.z + (fDiffBlend * fRateBlend);		// 求める値

				// 向きを調整
				if (rotDest.z > D3DX_PI)
				{
					rotDest.z -= D3DX_PI * 2;
				}
				else if (rotDest.z < -D3DX_PI)
				{
					rotDest.z += D3DX_PI * 2;
				}
			}
			else
			{// ブレンドなし
				// キー情報から位置・向きを算出
				// 位置X
				fDiffKey = pKeyNext->fPosX - pKey->fPosX;
				posDest.x = pKey->fPosX + (fDiffKey * fRateKey);

				// 位置Y
				fDiffKey = pKeyNext->fPosY - pKey->fPosY;
				posDest.y = pKey->fPosY + (fDiffKey * fRateKey);

				// 位置Z
				fDiffKey = pKeyNext->fPosZ - pKey->fPosZ;
				posDest.z = pKey->fPosZ + (fDiffKey * fRateKey);

				// 向きX
				fDiffKey = pKeyNext->fRotX - pKey->fRotX;
				rotDest.x = pKey->fRotX + (fDiffKey * fRateKey);

				// 向きを調整
				if (rotDest.x > D3DX_PI)
				{
					rotDest.x -= D3DX_PI * 2;
				}
				else if (rotDest.x < -D3DX_PI)
				{
					rotDest.x += D3DX_PI * 2;
				}

				// 向きY
				fDiffKey = pKeyNext->fRotY - pKey->fRotY;
				rotDest.y = pKey->fRotY + (fDiffKey * fRateKey);

				// 向きを調整
				if (rotDest.y > D3DX_PI)
				{
					rotDest.y -= D3DX_PI * 2;
				}
				else if (rotDest.y < -D3DX_PI)
				{
					rotDest.y += D3DX_PI * 2;
				}

				// 向きZ
				fDiffKey = pKeyNext->fRotZ - pKey->fRotZ;
				rotDest.z = pKey->fRotZ + (fDiffKey * fRateKey);

				// 向きを調整
				if (rotDest.z > D3DX_PI)
				{
					rotDest.z -= D3DX_PI * 2;
				}
				else if (rotDest.z < -D3DX_PI)
				{
					rotDest.z += D3DX_PI * 2;
				}
			}

			// パーツの位置・向きを設定
			pModel->pos = posDest + pComputer->aModel[nCntModel].posOff;
			pModel->rot = rotDest + pComputer->aModel[nCntModel].rotOff;
		}

		if (pComputer->bBlendMotion == true)
		{// ブレンドあり
			pComputer->nCounterMotion++;

			if (pComputer->nCounterMotion >= pComputer->aMotionInfo[pComputer->motionType].aKeyInfo[pComputer->nKey].nFrame)
			{// 再生フレーム数に達したら現在のキーを1つ進める
				pComputer->nCounterMotion = 0;	// カウンターを戻す

				// ループかどうか
				if (pComputer->bLoopMotion == true)
				{// ループモーション
					pComputer->nKey = (pComputer->nKey + 1) % (pComputer->aMotionInfo[pComputer->motionType].nNumKey);
				}
				else
				{// ループしないモーション
					pComputer->nKey++;

					if (pComputer->nKey >= pComputer->aMotionInfo[pComputer->motionType].nNumKey - 1)
					{// キーの総数を超えた
						pComputer->nKey = pComputer->aMotionInfo[pComputer->motionType].nNumKey - 1;

						pComputer->bFinishMotion = true;
					}
				}
			}

			pComputer->nCounterMotionBlend++;

			if (pComputer->nCounterMotionBlend >= pComputer->aMotionInfo[pComputer->motionTypeBlend].aKeyInfo[pComputer->nKeyBlend].nFrame)
			{// 再生フレーム数に達したら現在のキーを1つ進める
				pComputer->nCounterMotionBlend = 0;	// カウンターを戻す

				// ループかどうか
				if (pComputer->bLoopMotionBlend == true)
				{// ループモーション
					pComputer->nKeyBlend = (pComputer->nKeyBlend + 1) % (pComputer->aMotionInfo[pComputer->motionTypeBlend].nNumKey);
				}
				else
				{// ループしないモーション
					pComputer->nKeyBlend++;

					if (pComputer->nKeyBlend >= pComputer->aMotionInfo[pComputer->motionTypeBlend].nNumKey - 1)
					{// キーの総数を超えた
						pComputer->nKeyBlend = pComputer->aMotionInfo[pComputer->motionTypeBlend].nNumKey - 1;

						pComputer->bFinishMotion = true;
					}
				}
			}

			pComputer->nCounterBlend++;

			if (pComputer->nCounterBlend >= pComputer->nFrameBlend)
			{// ブレンドフレームに到達
				// ブレンドモーションを現在のモーションに設定
				pComputer->motionType = pComputer->motionTypeBlend;
				pComputer->bLoopMotion = pComputer->bLoopMotionBlend;
				pComputer->nNumKey = pComputer->nNumKeyBlend;
				pComputer->nKey = pComputer->nKeyBlend;
				pComputer->nCounterMotion = pComputer->nCounterMotionBlend;
				//pComputer->bFinishMotion = true;

				pComputer->nKeyBlend = 0;
				pComputer->nCounterMotionBlend = 0;
				pComputer->bBlendMotion = false;
				pComputer->nCounterBlend = 0;
				pComputer->nFrameBlend = 0;
			}
		}
		else
		{// ブレンドなし
			pComputer->nCounterMotion++;

			if (pComputer->nCounterMotion >= pComputer->aMotionInfo[pComputer->motionType].aKeyInfo[pComputer->nKey].nFrame)
			{// 再生フレーム数に達したら現在のキーを1つ進める
				pComputer->nCounterMotion = 0;	// カウンターを戻す

				// ループかどうか
				if (pComputer->bLoopMotion == true)
				{// ループモーション
					pComputer->nKey = (pComputer->nKey + 1) % (pComputer->aMotionInfo[pComputer->motionType].nNumKey);
				}
				else
				{// ループしないモーション
					pComputer->nKey++;

					if (pComputer->nKey >= pComputer->aMotionInfo[pComputer->motionType].nNumKey - 1)
					{// キーの総数を超えた
						pComputer->nKey = pComputer->aMotionInfo[pComputer->motionType].nNumKey - 1;

						pComputer->bFinishMotion = true;
					}
				}
			}
		}
	}
}

//=============================================================================
// CPUモーションの設定処理
//=============================================================================
void SetMotionComputer(int nIdx, MOTIONTYPE motionType, bool bBlendMotion, int nFrameBlend)
{
	Computer* pComputer = GetComputer();

	for (int nCntComputer = 0; nCntComputer < MAX_COMPUTER; nCntComputer++, pComputer++)
	{
		if (pComputer->motionTypeBlend != motionType)
		{// 違うモーションが設定されたときだけ
			if (bBlendMotion == true)
			{// ブレンドあり
				pComputer->motionTypeBlend = motionType;
				pComputer->bLoopMotionBlend = pComputer->aMotionInfo[motionType].bLoop;
				pComputer->nNumKeyBlend = pComputer->aMotionInfo[motionType].nNumKey;
				pComputer->nKeyBlend = 0;
				pComputer->nCounterMotionBlend = 0;
				pComputer->bFinishMotion = false;

				pComputer->bBlendMotion = bBlendMotion;
				pComputer->nFrameBlend = nFrameBlend;
				pComputer->nCounterBlend = 0;
			}
			else
			{// ブレンドなし
				pComputer->motionType = motionType;
				pComputer->bLoopMotion = pComputer->aMotionInfo[motionType].bLoop;
				pComputer->nNumKey = pComputer->aMotionInfo[motionType].nNumKey;
				pComputer->nKey = 0;
				pComputer->nCounterMotion = 0;
				pComputer->bFinishMotion = false;

				pComputer->bBlendMotion = bBlendMotion;

				// 全モデル(パーツ)の初期設定
				for (int nCntModel = 0; nCntModel < pComputer->nNumModel; nCntModel++)
				{
					pComputer->aModel[nCntModel].pos.x = pComputer->aMotionInfo[motionType].aKeyInfo[0].aKey[0].fPosX;
					pComputer->aModel[nCntModel].pos.y = pComputer->aMotionInfo[motionType].aKeyInfo[0].aKey[0].fPosY;
					pComputer->aModel[nCntModel].pos.z = pComputer->aMotionInfo[motionType].aKeyInfo[0].aKey[0].fPosZ;

					pComputer->aModel[nCntModel].rot.x = pComputer->aMotionInfo[motionType].aKeyInfo[0].aKey[0].fRotX;
					pComputer->aModel[nCntModel].rot.y = pComputer->aMotionInfo[motionType].aKeyInfo[0].aKey[0].fRotY;
					pComputer->aModel[nCntModel].rot.z = pComputer->aMotionInfo[motionType].aKeyInfo[0].aKey[0].fRotZ;
				}
			}
		}
	}
}