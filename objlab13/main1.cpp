//#include <iostream>
//#include <fstream>
//#include <regex>
//#include <string>
//#include "Affind3D.h"
//
//int main1()
//{
//    setlocale(LC_ALL, "rus"); // корректное отображение Кириллицы
//
//    Point point(1, 2, 3);
//    char buff[1000]; // буфер промежуточного хранения считываемого из файла текста
//    ifstream fin("capsule1.obj"); // открыли файл для чтения
//
//    regex myregex;
//    int mode = 0;
//
//    Polyhedron polyhedron;
//
//    while (!fin.eof())
//    {
//        fin.getline(buff, 1000); // считали строку из файла
//        string s = buff;
//        if (s[0] == '#')
//            continue;
//        if (s[0] == 'v')
//        {
//            if (s[1] == ' ')
//            {
//                myregex = regex("v (\-?\\d+,\\d+) (\-?\\d+,\\d+) (\-?\\d+,\\d+)");
//                mode = 0;
//            }
//            else if (s[1] == 'n')
//            {
//                myregex = regex("vn (\-?\\d+,\\d+) (\-?\\d+,\\d+) (\-?\\d+,\\d+)");
//                mode = 1;
//            }
//            else if (s[1] == 't')
//            {
//                myregex = regex("vt (\-?\\d+,\\d+) (\-?\\d+,\\d+)");
//                mode = 2;
//            }
//        }
//        else if (s[0] == 'f')
//        {
//            myregex = regex("f (\\d+/\\d+/\\d+) (\\d+/\\d+/\\d+) (\\d+/\\d+/\\d+)");
//            mode = 3;
//        }
//        auto words_begin = sregex_iterator(s.begin(), s.end(), myregex);
//        auto words_end = sregex_iterator();
//        for (sregex_iterator i = words_begin; i != words_end; i++)
//        {
//            smatch match = *i;
//            if (mode == 0)
//            {
//                Point p(stod(match[1]), stod(match[2]), stod(match[3]));
//                polyhedron.pointList.push_back(p);
//            }
//            else if (mode == 1)
//            {
//                Point p(stod(match[1]), stod(match[2]), stod(match[3]));
//                polyhedron.normalList.push_back(p);
//            }
//            else if (mode == 2)
//            {
//                Point p(stod(match[1]), stod(match[2]), 0);
//                polyhedron.texturePoint.push_back(p);
//            }
//            else if (mode == 3)
//            {
//                vector<tuple<int, int, int>> polygon = vector<tuple<int, int, int>>();
//                for (int j = 1; j < match.size(); j++)
//                {
//                    regex point = regex("(\\d+)/(\\d+)/(\\d+)");
//                    string s0 = match[j];
//                    auto matchpoint = sregex_iterator(s0.begin(), s0.end(), point);
//                    //smatch matchpoint = *pointm;
//                    polygon.push_back(make_tuple(stoi((*matchpoint)[1])-1, stoi((*matchpoint)[2]) - 1, stoi((*matchpoint)[3]) - 1));
//                }
//                polyhedron.polygons.push_back(polygon);
//            }
//        }
//    }
//    int* indices = new int[polyhedron.polygons.size()*3];
//    int pointer = 0;
//    for (int i = 0; i < polyhedron.polygons.size(); i++)
//    {
//        for (int j = 0; j < polyhedron.polygons[i].size(); j++)
//        {
//            indices[pointer] = get<0>(polyhedron.polygons[i][j]);
//            ++pointer;
//        }
//    }
//    float** vertices = new float*[polyhedron.pointList.size()];
//    for (int i = 0; i < polyhedron.pointList.size(); i++)
//    {
//        vertices[i] = new float[3];
//        vertices[i][0] = polyhedron.pointList[i].x;
//        vertices[i][1] = polyhedron.pointList[i].y;
//        vertices[i][2] = polyhedron.pointList[i].z;
//    }
//    float** normals = new float* [polyhedron.normalList.size()];
//    for (int i = 0; i < polyhedron.normalList.size(); i++)
//    {
//        normals[i] = new float[3];
//        normals[i][0] = polyhedron.normalList[i].x;
//        normals[i][1] = polyhedron.normalList[i].y;
//        normals[i][2] = polyhedron.normalList[i].z;
//    }
//
//    cout << indices[0] << " " << indices[1] << " " << indices[2] << endl;
//    cout << vertices[0][0] << " " << vertices[0][1] << " " << vertices[0][2] << endl;
//    cout << normals[0][0] << " " << normals[0][1] << " " << normals[0][2] << endl;
//    fin.close(); // закрываем файл
//    system("pause");
//    return 0;
//}