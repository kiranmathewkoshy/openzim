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
#include <zim/writer/zimpatch.h>
#include <zim/blob.h>
#include <zim/article.h>
#include <zim/file.h>
#include <zim/fileiterator.h>
#include <zim/uuid.h>
#include <list>
#include <limits>
#include <algorithm>





void displayHelp()
{
    std::cout<<"\nzimpatch"
             "\nA tool to compute the end_file using a atart_file and a diff_file."
             "\nUsage: zimpatch [start_file] [diff_file] [output file]  \n";
    return;
}

//A series of checks to be executed before the patch process begins, in order to confirm that the correct files are being used.
bool isAdditionalMetadata(std::string url)
{
    if(url=="M/dlist")
        return true;
    if(url=="M/startfileuid")
        return true;
    if(url=="M/endfileuid")
        return true;
    if(url=="M/mainaurl")
        return true;
    if(url=="M/layoutaurl")
        return true;
    if(url=="M/redirectlist")
        return true;
    return false;
}

bool checkDiffFile(std::string startFileName, std::string diffFileName)
{
    zim::File startFile(startFileName);
    zim::File diffFile(diffFileName);
    std::vector<std::string >additionalMetadata;
    additionalMetadata.resize(6);
    additionalMetadata[0]="M/dlist";
    additionalMetadata[1]="M/startfileuid";
    additionalMetadata[2]="M/endfileuid";
    additionalMetadata[3]="M/mainaurl";
    additionalMetadata[4]="M/layoutaurl";
    additionalMetadata[5]="M/redirectlist";

    //Search in the ZIM file if the above articles are present:
    for(unsigned int i=0; i<additionalMetadata.size(); i++)
    {
        if(!diffFile.findx(additionalMetadata[i]).first)        //If the article was not found in the file.
            return false;
    }

    //Check the UID of startFile and the value stored in startfileuid
    std::string startfileUID1;
    std::string startFileUID2=diffFile.getArticleByUrl(additionalMetadata[1]).getPage();
    std::ostringstream ss;
    ss<<startFile.getFileheader().getUuid();
    startfileUID1=ss.str();
    return startfileUID1==startFileUID2;
}


int main(int argc, char* argv[])
{

    //Parsing arguments
    //There will be only three arguments, so no detailed parsing is required.
    std::cout<<"zimpatch\nVERSION "<<VERSION<<"\n"<<std::flush;
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
        displayHelp();
        return -1;
    }

    //Strings containing the filenames of the start_file, diff_file and end_file.
    std::string start_filename =argv[1];
    std::string diff_filename =argv[2];
    std::string end_filename= argv[3];
    std::cout<<"\nStart File: "<<start_filename;
    std::cout<<"\nDiff File: "<<diff_filename;
    std::cout<<"\nEnd File: "<<end_filename<<"\n";
    try
    {
        //Callling zimwriter to create the diff_file
        if(!checkDiffFile(start_filename,diff_filename))
        {
            std::cout<<"\n[ERROR]: The diff file provided does not match the given start file.";
            return 0;
        }
        zim::zimPatch z(start_filename,diff_filename,end_filename,argc,argv);
        //Create the actual file.
        z.write();

    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}











