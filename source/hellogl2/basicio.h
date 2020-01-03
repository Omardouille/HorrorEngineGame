/*
    QMVC - Reference Implementation of paper:

    "Mean Value Coordinates for Quad Cages in 3D",
    jean-Marc Thiery, Pooran Memari and Tamy Boubekeur
    SIGGRAPH Asia 2018

    This program allows to compute QMVC for a set of 3D points contained
    in a cage made of quad and triangles, as well as other flavors of
    space coordinates for cages (MVC, SMVC, GC, MEC). It comes also with
    a 3D viewer which helps deforming a mesh with a cage.

    Copyright (C) 2018  jean-Marc Thiery, Pooran Memari and Tamy Boubekeur

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef BASICIO_H
#define BASICIO_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QDataStream>
#include <cfloat>
#include <cmath>
#include <cassert>
#include <qdebug.h>




namespace OBJIO{





template< class point_t , class point2d_t, class int_type_t, class ints_t > bool open(
        const std::string & filename,
        std::vector<point_t> & vertices,
        std::vector< std::vector< ints_t > > & faces,
        std::vector<point_t> & normals,
        std::vector<point2d_t> & texcoords,
        bool convertToTriangles = true,
        bool randomize = false,
        bool convertEdgesToDegenerateTriangles = true)
{


    std::ifstream myfile;
    myfile.open(filename.c_str());
    if (!myfile.is_open())
    {
        std::cout << filename << " cannot be opened" << std::endl;
        return false;
    }

    vertices.clear();
    faces.clear();
    normals.clear();

    while( myfile.good() )
    {
        std::string line;
        getline (myfile,line);
        QString QTLine = QString::fromStdString( line );
        QRegExp reg("\\s+");
        QStringList lineElements = QTLine.split(reg);

        if(  lineElements.size() > 0  )
        {
            QString elementType = lineElements[0];
            // vertex
            if ( elementType == QString("v") )
            {
                vertices.push_back(point_t( lineElements[1].toDouble() , lineElements[2].toDouble() , lineElements[3].toDouble() ));
            }
            // normals
            if ( elementType == QString("vn") )
            {
                normals.push_back(point_t( lineElements[1].toDouble() , lineElements[2].toDouble() , lineElements[3].toDouble() ));
            }

            if ( elementType == QString("vt") )
            {
                texcoords.push_back(point2d_t( lineElements[1].toDouble() , lineElements[2].toDouble()));
            }
            else if ( elementType == QString("f") )
            {
                std::vector< ints_t > vhandles;
                //std::vector< int_type_t > vhandles_normals;
                for( int i = 1 ; i < lineElements.size() ; ++i )
                {
                    QStringList FaceElements = lineElements[i].split("/", QString::SkipEmptyParts);
                    if( FaceElements.size() > 0 ) {
                     //   vhandles.push_back( (int_type_t)( (abs(FaceElements[0].toInt()) - 1) ) );
                        if(FaceElements.size() <= 2)
                            //vhandles_normals.push_back( (int_type_t)( (abs(FaceElements[1].toInt()) - 1) ) );
                            vhandles.push_back( { (int_type_t)( (abs(FaceElements[0].toInt()) - 1) ), (int_type_t)( (abs(FaceElements[1].toInt()) - 1) ), (int_type_t)0 } );
                        else
                            //vhandles_normals.push_back( (int_type_t)( (abs(FaceElements[2].toInt()) - 1) ) );
                            vhandles.push_back( { (int_type_t)( (abs(FaceElements[0].toInt()) - 1) ), (int_type_t)( (abs(FaceElements[2].toInt()) - 1) ), (int_type_t)( (abs(FaceElements[1].toInt()) - 1) )} );
                    }
                    //qDebug() << FaceElements[0].toInt() << "-" << FaceElements[1].toInt() << "-" << FaceElements.size() << "test\n";
                }

                if (vhandles.size()>3)
                {
                    if( convertToTriangles )
                    {
                        //model is not triangulated, so let us do this on the fly...
                        //to have a more uniform mesh, we add randomization
                        unsigned int k=(randomize)?(rand()%vhandles.size()):0;
                        for (unsigned int i=0;i<vhandles.size()-2;++i)
                        {
                            std::vector< ints_t > tri(3);
                            tri[0]=  vhandles[(k+0)%vhandles.size()];
                            tri[1]=   vhandles[(k+i+1)%vhandles.size()];
                            tri[2]=   vhandles[(k+i+2)%vhandles.size()];
                            faces.push_back(tri);
                        }
                    }
                    else
                        faces.push_back(vhandles);
                }
                else if (vhandles.size()==3)
                {
                    faces.push_back(vhandles);
                }
                else if (vhandles.size()==2)
                {
                    if( convertEdgesToDegenerateTriangles )
                    {
                        printf("Unexpected number of face vertices (2). Converting edge to degenerate triangle");
                        vhandles.push_back(vhandles[1]);
                       // vhandles_normals.push_back(vhandles_normals[1]);
                        faces.push_back(vhandles);
                    }
                    else
                        printf("Unexpected number of face vertices (2). Ignoring face");
                }
            }
        }
    }
    myfile.close();

    return true;
}



}




#endif // BASICIO_H
