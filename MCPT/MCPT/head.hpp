#pragma once
#include <algorithm>
#include <iostream>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <assimp/Importer.hpp>      // 导入器在该头文件中定义
#include <assimp/scene.h>           // 读取到的模型数据都放在scene中
#include <assimp/postprocess.h>     // 该头文件中包含后处理的标志位定义
#include <string>
#include <vector>
using std::string;
using std::cout;
using std::endl;
#include <tchar.h>
#include "math_tools.hpp"
#include "vector3.hpp"
using Color = Vector3;
struct Vertex {
	Vector3 Position;
	Vector3 Normal;
};
#include "Ray.hpp"
#include "AABB.hpp"
#include "model.hpp"
#include "imageio.hpp"
#include "camera.hpp"
#include "IntersectTriangle.hpp"
#include "KDTree.hpp"
#include <time.h>
#include <thread>