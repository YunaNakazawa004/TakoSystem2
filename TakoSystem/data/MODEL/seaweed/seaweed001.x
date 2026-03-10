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
 20;
 0.00000;42.44352;0.00000;,
 3.36156;-1.09064;0.13512;,
 0.00000;-1.09064;-3.95372;,
 0.00000;42.44352;0.00000;,
 -3.95372;-1.09064;0.00000;,
 3.36156;-1.09064;0.13512;,
 0.00000;42.44352;0.00000;,
 0.00000;-1.09064;-3.95372;,
 0.65280;35.32576;-0.33736;,
 -11.92019;60.90606;0.47699;,
 0.17020;95.47948;0.09896;,
 12.83425;59.26898;-0.80458;,
 -59.11260;18.68188;-6.80556;,
 -24.86821;28.78761;-2.33833;,
 0.21340;23.64096;-0.14428;,
 -23.54082;14.41038;-3.43974;,
 -1.00704;13.73180;-0.01148;,
 22.76500;22.96088;-3.19021;,
 57.94140;18.69092;-9.44664;,
 23.75085;8.58211;-4.61257;;
 
 9;
 3;0,1,2;,
 3;3,4,5;,
 3;6,7,4;,
 4;8,9,10,11;,
 4;11,10,9,8;,
 4;12,13,14,15;,
 4;15,14,13,12;,
 4;16,17,18,19;,
 4;19,18,17,16;;
 
 MeshMaterialList {
  1;
  9;
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
  9;
  0.771179;0.057580;-0.634009;,
  -0.018468;0.001677;0.999828;,
  -0.705653;0.064087;-0.705653;,
  -0.051253;0.006797;-0.998663;,
  0.051253;-0.006797;0.998663;,
  0.104128;0.085523;-0.990880;,
  -0.104128;-0.085523;0.990880;,
  -0.164541;0.085920;-0.982621;,
  0.164541;-0.085920;0.982621;;
  9;
  3;0,0,0;,
  3;1,1,1;,
  3;2,2,2;,
  4;3,3,3,3;,
  4;4,4,4,4;,
  4;5,5,5,5;,
  4;6,6,6,6;,
  4;7,7,7,7;,
  4;8,8,8,8;;
 }
 MeshTextureCoords {
  20;
  0.125000;0.000000;,
  0.250000;1.000000;,
  0.000000;1.000000;,
  0.625000;0.000000;,
  0.750000;1.000000;,
  0.500000;1.000000;,
  0.875000;0.000000;,
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
