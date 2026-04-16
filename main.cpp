#include <Novice.h>
#include <cmath>
#include <utility>

struct Matrix4x4 {
	float m[4][4];
};

// 1. 行列の加法
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2);
// 2. 行列の減法
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2);
// 3. 行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);
// 4. 逆行列
Matrix4x4 Inverse(const Matrix4x4& m);
// 5. 転置行列
Matrix4x4 Transpose(const Matrix4x4& m);
// 6. 単位行列の作成
Matrix4x4 MakeIdentity4x4();

static const int kRowHeight = 20;
static const int kColumnWidth = 60;
void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label) {
	Novice::ScreenPrintf(x, y, "%s", label);
	for (int row = 0; row < 4; ++row) {
		for (int column = 0; column < 4; ++column) {
			Novice::ScreenPrintf(x + column * kColumnWidth, y + (row + 1) * kRowHeight, "%6.02f", matrix.m[row][column]);
		}
	}
}

const char kWindowTitle[] = "LE2A_12_スズキ_ダイスケ_MT3_00_02";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	Matrix4x4 m1 = {3.2f, 0.7f, 9.6f, 4.4f, 5.5f, 1.3f, 7.8f, 2.1f, 6.9f, 8.0f, 2.6f, 1.0f, 0.5f, 7.2f, 5.1f, 3.3f};

	Matrix4x4 m2 = {4.1f, 6.5f, 3.3f, 2.2f, 8.8f, 0.6f, 9.9f, 7.7f, 1.1f, 5.5f, 6.6f, 0.0f, 3.3f, 9.9f, 8.8f, 2.2f};

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		Matrix4x4 resultAdd = Add(m1, m2);
		Matrix4x4 resultMultipry = Multiply(m1, m2);
		Matrix4x4 resultSubtract = Subtract(m1, m2);
		Matrix4x4 inverseM1 = Inverse(m1);
		Matrix4x4 inverseM2 = Inverse(m2);
		Matrix4x4 transposeM1 = Transpose(m1);
		Matrix4x4 transposeM2 = Transpose(m2);
		Matrix4x4 identity = MakeIdentity4x4();

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		MatrixScreenPrintf(0, 0, resultAdd, "Add");
		MatrixScreenPrintf(0, kRowHeight * 5, resultSubtract, "Subtract");
		MatrixScreenPrintf(0, kRowHeight * 5 * 2, resultMultipry, "Multipry");
		MatrixScreenPrintf(0, kRowHeight * 5 * 3, inverseM1, "inverseM1");
		MatrixScreenPrintf(0, kRowHeight * 5 * 4, inverseM2, "inverseM2");
		MatrixScreenPrintf(kColumnWidth * 5, 0, transposeM1, "transposeM1");
		MatrixScreenPrintf(kColumnWidth * 5, kRowHeight * 5, transposeM2, "transposeM2");
		MatrixScreenPrintf(kColumnWidth * 5, kRowHeight * 5 * 2, identity, "identity");

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}

// 1. 行列の加法
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result{};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = m1.m[i][j] + m2.m[i][j];
		}
	}
	return result;
};

// 2. 行列の減法
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result{};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = m1.m[i][j] - m2.m[i][j];
		}
	}
	return result;
};

// 3. 行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result{};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = m1.m[i][0] * m2.m[0][j] + m1.m[i][1] * m2.m[1][j] + m1.m[i][2] * m2.m[2][j] + m1.m[i][3] * m2.m[3][j];
		}
	}
	return result;
};

// 4. 逆行列
Matrix4x4 Inverse(const Matrix4x4& m) {
	Matrix4x4 a = m;                   // 作業用
	Matrix4x4 inv = MakeIdentity4x4(); // 単位行列

	for (int i = 0; i < 4; i++) {
		// ピボット選択（0 なら失敗）
		float pivot = a.m[i][i];
		if (fabs(pivot) < 1e-6f) {
			// ピボットが小さすぎる場合、行を交換する
			for (int r = i + 1; r < 4; r++) {
				if (fabs(a.m[r][i]) > 1e-6f) {
					// std::swap ... 変数同士の値を入れ替える
					std::swap(a.m[i], a.m[r]);
					std::swap(inv.m[i], inv.m[r]);
					pivot = a.m[i][i];
					break;
				}
			}
		}

		// それでも pivot が 0 なら逆行列なし
		if (fabs(pivot) < 1e-6f) {
			return MakeIdentity4x4(); // 失敗時の代替（適宜変更）
		}

		// ピボット行を 1 に正規化
		float invPivot = 1.0f / pivot;
		for (int j = 0; j < 4; j++) {
			a.m[i][j] *= invPivot;
			inv.m[i][j] *= invPivot;
		}

		// 他の行からピボット列を消去
		for (int r = 0; r < 4; r++) {
			if (r == i)
				continue;
			float factor = a.m[r][i];
			for (int c = 0; c < 4; c++) {
				a.m[r][c] -= factor * a.m[i][c];
				inv.m[r][c] -= factor * inv.m[i][c];
			}
		}
	}

	return inv;
}

// 5. 転置行列
Matrix4x4 Transpose(const Matrix4x4& m) {
	Matrix4x4 result{};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = m.m[j][i];
		}
	}
	return result;
};

// 6. 単位行列の作成
Matrix4x4 MakeIdentity4x4() {
	Matrix4x4 result{};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = (i == j) ? 1.0f : 0.0f;
		}
	}
	return result;
};
