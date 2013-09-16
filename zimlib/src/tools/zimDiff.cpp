/*
 * Copyright (C) 2013 Kiran Mathew Koshy
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */


#define VERSION "0.6.0.0"
#include <iostream>
#include <sstream>
#include <vector>
#include <zim/writer/zimcreator.h>
#include <zim/writer/zimdiff.h>
#include <zim/blob.h>
#include <zim/article.h>
#include <zim/file.h>
#include <zim/fileiterator.h>
#include <list>
#include <algorithm>



void displayHelp()
{
    std::cout<<"\nzimdiff"
             "\nA tool to obtain the diff_file between two ZIM files, in order to facilitate incremental updates."
             "\nUsage: zimdiff [start_file] [end_file] [output file]  \n";
    return;
}

int main(int argc, char* argv[])
{

    //Parsing arguments
    //There will be only two arguments, so no detailed parsing is required.
    std::cout<<"zimdiff\n";
    for(int i=0; i<argc; i++)
    {
        if(std::string(argv[i])=="-h")
        {
            displayHelp();
            return 0;
        }

        if(std::string(argv[i])=="-H")
        {
            displayHelp();
            return 0;
        }

        if(std::string(argv[i])=="--help")
        {
            displayHelp();
            return 0;
        }
    }
    if(argc<4)
    {
        std::cout<<"\n[ERROR] Not enough Arguments provided\n";
        return -1;
    }
    std::string filename_1 =argv[1];
    std::string filename_2 =argv[2];
    std::string op_file= argv[3];
    try
    {

       zim::zimDiff z(filename_1,filename_2,op_file,argc,argv);
       z.write();

    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}


















