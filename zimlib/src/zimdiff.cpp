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
#include <zim/blob.h>
#include <zim/article.h>
#include <zim/file.h>
#include <zim/fileiterator.h>
#include <zim/writer/zimdiff.h>
#include <list>
#include <algorithm>


namespace zim
{
zimDiff::ArticleSource::ArticleSource(std::string filename_1="",std::string filename_2="")
{
    dlistToggle=false;
    startIdToggle=false;
    endIdToggle=false;
    mainAurlToggle=false;
    layoutAurlToggle=false;
    redirectListToggle=false;
    file_1 = zim::File(filename_1);
    file_2 = zim::File(filename_2);
    fileSize = file_2.getFileheader().getArticleCount();
    deleteList.clear();
    itr=file_2.begin();
    std::string rdlist="";
    //Scanning Data from files, generating list of articles to be deleted
    for(zim::File::const_iterator it = file_1.begin(); it != file_1.end(); ++it)
    {
        if(file_2.getArticleByUrl(it->getLongUrl()).getIndex()==std::numeric_limits<unsigned int>::max())
            deleteList.push_back(it->getLongUrl());
    }
    for(zim::File::const_iterator it=file_2.begin(); it!=file_2.end(); ++it)
    {
        if(it->isRedirect())
        {
            rdlist+=it->getLongUrl()+"\n"+it->getRedirectArticle().getLongUrl()+"\n";
        }
    }
    //Setting data in dlist,startFileUID,endFileUID, etc.

    //dlist article.
    //List of articles to be deleted from start_file.
    std::string dlListText="";
    for(std::list<std::string >::iterator it=deleteList.begin(); it!=deleteList.end(); ++it)
    {
        dlListText+=*it+"\n";
    }
    dlist.Title="dlist";
    dlist.Url="dlist";
    dlist.MimeType="text/plain";
    dlist._data=dlListText;
    dlist.nm='M';
    dlist._id=toString((long long)fileSize+1);
    dlist.IsRedirect=false;
    dlist.RedirectAid="";

    //StartFileUID
    //contains the UID of the start_file.
    startFileUID.Title="startfileuid";
    startFileUID.Url="startfileuid";
    startFileUID.MimeType="text/plain";
    const char *s=file_1.getFileheader().getUuid().data;
    std::string st="";
    for(int i=0; i<16; i++)
    {
        st+=toString((int)s[i]);
        st+="\n";
    }
    startFileUID._data=st;
    startFileUID.nm='M';
    startFileUID._id=toString((long long)fileSize+2);
    startFileUID.IsRedirect=false;
    startFileUID.RedirectAid="";

    //EndFileUID
    //contains the UID of the end_file.
    endFileUID.Title="endfileuid";
    endFileUID.Url="endfileuid";
    endFileUID.MimeType="text/plain";
    s=file_2.getFileheader().getUuid().data;
    st="";
    for(int i=0; i<16; i++)
    {
        st+=toString((int)s[i]);
        st+="\n";
    }
    endFileUID._data=st;
    endFileUID.nm='M';
    endFileUID._id=toString((long long)fileSize+3);
    endFileUID.IsRedirect=false;
    endFileUID.RedirectAid="";

    //Metadata article storing the MAIN Article for the new ZIM file.
    mainAurl.Title="mainaurl";
    mainAurl.Url="mainaurl";
    mainAurl.MimeType="text/plain";
    if(file_2.getFileheader().hasMainPage())
        mainAurl._data=file_2.getArticle(file_2.getFileheader().getMainPage()).getLongUrl();
    else
        mainAurl._data="";
    mainAurl.nm='M';
    mainAurl._id=toString((long long)fileSize+4);
    mainAurl.IsRedirect=false;
    mainAurl.RedirectAid="";


    //Metadata entry storing the Layout Article for the new ZIM file.
    layoutAurl.Title="layoutaurl";
    layoutAurl.Url="layoutaurl";
    layoutAurl.MimeType="text/plain";
    if(file_2.getFileheader().hasLayoutPage())
        layoutAurl._data=file_2.getArticle(file_2.getFileheader().getLayoutPage()).getLongUrl();
    else
        layoutAurl._data="";
    layoutAurl.nm='M';
    layoutAurl._id=toString((long long)fileSize+5);
    layoutAurl.IsRedirect=false;
    layoutAurl.RedirectAid="";



    //Meatadata entry for storing the redirect entries.
    redirectList.Title="redirectlist";
    redirectList.Url="redirectlist";
    redirectList.MimeType="text/plain";
    redirectList._data=rdlist;
    redirectList.nm='M';
    redirectList._id=toString((long long)fileSize+6);
    redirectList.IsRedirect=false;
    redirectList.RedirectAid="";
    return;
}

const zim::writer::Article* zimDiff::ArticleSource::getNextArticle()
{
    //Add All articles in file_2 that are not in file_1 and those that are not the same in file_1

    //Articles are added frm file_2.
    //loop till an article read to be added is found.
    bool found=false;
    bool redirect=false;
    uint16_t mime=0;
    while(itr!=file_2.end())        //While not reaching EOF
    {
        //irt is the file pointer in file_2
        if(file_1.getArticleByUrl(itr->getLongUrl()).getIndex()==std::numeric_limits<unsigned int>::max()) //If the article is not present in FILe 1
        {
            tempArticle=Article(*itr);
            if(itr->isRedirect())
                redirect=true;
            mime=itr->getLibraryMimeType();
            ++itr;
            found=true;
            break;
        }
        //if this place of the loop is reached, it is sure that the article is presentin file_1
        if(!(file_1.getArticleByUrl(itr->getLongUrl()).getData()==itr->getData()))      //if the data stored in the same article is different in file_1 and 2
        {
            tempArticle=Article(*itr);
            if(itr->isRedirect())
                redirect=true;
            mime=itr->getLibraryMimeType();
            ++itr;
            found=true;
            break;
        }
        ++itr;
    }
    if(found)
    {
        if(redirect)
            tempArticle.setRedirect();
        return &tempArticle;
    }
    //Add Metadata articles.

    //Add list of articles to be deleted.
    if(!dlistToggle)
    {
        dlistToggle=true;
        return &dlist;
    }


    //Add startFile UID
    if(!startIdToggle)
    {
        startIdToggle=true;
        return &startFileUID;
    }


    //Add endFile UID
    if(!endIdToggle)
    {
        endIdToggle=true;
        return &endFileUID;
    }

    if(!mainAurlToggle)
    {
        mainAurlToggle=true;
        return &mainAurl;
    }

    if(!layoutAurlToggle)
    {
        layoutAurlToggle=true;
        return &layoutAurl;
    }

    if(!redirectListToggle)
    {
        redirectListToggle=true;
        return &redirectList;
    }
    //Once all Articles have been added, reset list pointers, so that the Data can be read.
    return 0;
}

zim::Blob zimDiff::ArticleSource::getData(const std::string& aid)
{
    // Convert the string ID to an integer.
    int id=atoi(aid.c_str());
    if(id<fileSize)
        return file_2.getArticle(id).getData();
    if(id==fileSize+1)
    {
        return dlist.data();
    }

    if(id==fileSize+2)
    {
        return startFileUID.data();
    }

    if(id==fileSize+3)
    {
        return endFileUID.data();
    }
    if(id==fileSize+4)
    {
        return mainAurl.data();
    }
    if(id==fileSize+5)
    {
        return layoutAurl.data();
    }
    if(id==fileSize+6)
    {
        return redirectList.data();
    }
    std::cout<<"\nReturned null for: "<<aid<<std::flush;
    return zim::Blob("",0);
}

zimDiff::zimDiff(std::string filename_1,std::string filename_2,std::string filename_3,int argc,char *argv[])
    :
    c(argc,argv),
    src(filename_1,filename_2)
{
    start_file=filename_1;
    end_file=filename_2;
    diff_file=filename_3;

}

}

















