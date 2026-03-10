xof 0302txt 0064
template Header {
 <3D82AB43-62DA-11cf-AB39-0020AF71E433>
 WORD major;
 WORD minor;
 DWORD flags;
}

template Vector {
 <3D82AB5E-62DA-11cf-AB39-0020AF71E433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template Coords2d {
 <F6F23F44-7686-11cf-8F52-0040333594A3>
 FLOAT u;
 FLOAT v;
}

template Matrix4x4 {
 <F6F23F45-7686-11cf-8F52-0040333594A3>
 array FLOAT matrix[16];
}

template ColorRGBA {
 <35FF44E0-6C7C-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <D3E16E81-7835-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template IndexedColor {
 <1630B820-7842-11cf-8F52-0040333594A3>
 DWORD index;
 ColorRGBA indexColor;
}

template Boolean {
 <4885AE61-78E8-11cf-8F52-0040333594A3>
 WORD truefalse;
}

template Boolean2d {
 <4885AE63-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template MaterialWrap {
 <4885AE60-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template TextureFilename {
 <A42790E1-7810-11cf-8F52-0040333594A3>
 STRING filename;
}

template Material {
 <3D82AB4D-62DA-11cf-AB39-0020AF71E433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template MeshFace {
 <3D82AB5F-62DA-11cf-AB39-0020AF71E433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template MeshFaceWraps {
 <4885AE62-78E8-11cf-8F52-0040333594A3>
 DWORD nFaceWrapValues;
 Boolean2d faceWrapValues;
}

template MeshTextureCoords {
 <F6F23F40-7686-11cf-8F52-0040333594A3>
 DWORD nTextureCoords;
 array Coords2d textureCoords[nTextureCoords];
}

template MeshMaterialList {
 <F6F23F42-7686-11cf-8F52-0040333594A3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material]
}

template MeshNormals {
 <F6F23F43-7686-11cf-8F52-0040333594A3>
 DWORD nNormals;
 array Vector normals[nNormals];
 DWORD nFaceNormals;
 array MeshFace faceNormals[nFaceNormals];
}

template MeshVertexColors {
 <1630B821-7842-11cf-8F52-0040333594A3>
 DWORD nVertexColors;
 array IndexedColor vertexColors[nVertexColors];
}

template Mesh {
 <3D82AB44-62DA-11cf-AB39-0020AF71E433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

Header{
1;
0;
1;
}

Mesh {
 22;
 0.00000;79.44224;-3.95372;,
 3.96828;79.44224;0.00544;,
 3.96828;-0.95776;0.00544;,
 0.00000;-0.95776;-3.95372;,
 3.96828;79.44224;0.00544;,
 -3.95372;79.44224;0.00000;,
 -3.95372;-0.95776;0.00000;,
 3.96828;-0.95776;0.00544;,
 0.00000;79.44224;-3.95372;,
 0.00000;-0.95776;-3.95372;,
 -53.08068;64.87362;6.29278;,
 -0.27752;53.84684;-1.43340;,
 -26.53679;45.61048;3.65848;,
 -45.42825;14.19719;-8.10340;,
 -32.54913;37.70624;-4.74676;,
 1.76760;29.23800;0.25784;,
 33.76394;68.50718;-3.92686;,
 51.27938;47.97380;-1.88344;,
 -0.90428;61.42676;-0.13564;,
 48.90771;30.39836;7.16312;,
 15.43658;19.12800;3.79848;,
 -0.02400;34.01184;-1.19544;;
 
 11;
 4;0,1,2,3;,
 4;4,5,6,7;,
 4;5,8,9,6;,
 3;10,11,12;,
 3;10,12,11;,
 3;13,14,15;,
 3;13,15,14;,
 3;16,17,18;,
 3;16,18,17;,
 3;19,20,21;,
 3;19,21,20;;
 
 MeshMaterialList {
  1;
  11;
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0;;
  Material {
   0.220000;0.455200;0.248000;1.000000;;
   5.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;
  }
 }
 MeshNormals {
  11;
  0.706293;0.000000;-0.707920;,
  -0.000687;0.000000;1.000000;,
  -0.707107;0.000000;-0.707107;,
  -0.162471;-0.089499;-0.982646;,
  0.162471;0.089499;0.982646;,
  0.157280;0.054624;-0.986042;,
  -0.157280;-0.054624;0.986042;,
  -0.074612;-0.161574;-0.984036;,
  0.074612;0.161574;0.984036;,
  0.154047;-0.166750;-0.973891;,
  -0.154047;0.166750;0.973891;;
  11;
  4;0,0,0,0;,
  4;1,1,1,1;,
  4;2,2,2,2;,
  3;3,3,3;,
  3;4,4,4;,
  3;5,5,5;,
  3;6,6,6;,
  3;7,7,7;,
  3;8,8,8;,
  3;9,9,9;,
  3;10,10,10;;
 }
 MeshTextureCoords {
  22;
  0.000000;0.000000;,
  0.250000;0.000000;,
  0.250000;1.000000;,
  0.000000;1.000000;,
  0.500000;0.000000;,
  0.750000;0.000000;,
  0.750000;1.000000;,
  0.500000;1.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;;
 }
}
