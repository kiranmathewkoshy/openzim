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




namespace zim
{
        zimPatch::ArticleSource::ArticleSource(std::string start_filename="",std::string diff_filename="")
            :_next(0)
        {
            index=0;
            start_file=zim::File(start_filename);
            diff_file=zim::File(diff_filename);

            //Obtain endfileuid and convert it to a UUID object.
            std::string id=diff_file.getArticleByUrl("M/endfileuid").getPage();
            char tempArray[16];
            tempArray[0]=hexStringtoChar(id.substr(0,2));
            tempArray[1]=hexStringtoChar(id.substr(2,2));
            tempArray[2]=hexStringtoChar(id.substr(4,2));
            tempArray[3]=hexStringtoChar(id.substr(6,2));
            tempArray[4]=hexStringtoChar(id.substr(9,2));
            tempArray[5]=hexStringtoChar(id.substr(11,2));
            tempArray[6]=hexStringtoChar(id.substr(14,2));
            tempArray[7]=hexStringtoChar(id.substr(16,2));
            tempArray[8]=hexStringtoChar(id.substr(19,2));
            tempArray[9]=hexStringtoChar(id.substr(21,2));
            tempArray[10]=hexStringtoChar(id.substr(24,2));
            tempArray[11]=hexStringtoChar(id.substr(26,2));
            tempArray[12]=hexStringtoChar(id.substr(28,2));
            tempArray[13]=hexStringtoChar(id.substr(30,2));
            tempArray[14]=hexStringtoChar(id.substr(32,2));
            tempArray[15]=hexStringtoChar(id.substr(34,2));
            fileUid=zim::Uuid(tempArray);
            std::string tempString=diff_file.getArticleByUrl("M/mainaurl").getPage();
            if(diff_file.getArticleByUrl(tempString).getIndex()!=std::numeric_limits<unsigned int>::max())
            {
                mainAid=toString((long long)diff_file.getArticleByUrl(tempString).getIndex()+start_file.getFileheader().getArticleCount());
            }
            else if(start_file.getArticleByUrl(tempString).getIndex()!=std::numeric_limits<unsigned int>::max())
            {
                mainAid=toString((long long)start_file.getArticleByUrl(tempString).getIndex());
            }
            else
            {
                mainAid="";
            }
            if(mainAid!="")
            {

            }


            tempString=diff_file.getArticleByUrl("M/layoutaurl").getPage();
            if(diff_file.getArticleByUrl(tempString).getIndex()!=std::numeric_limits<unsigned int>::max())
            {
                layoutAid=toString((long long)diff_file.getArticleByUrl(tempString).getIndex()+start_file.getFileheader().getArticleCount());
            }
            else if(start_file.getArticleByUrl(tempString).getIndex()!=std::numeric_limits<unsigned int>::max())
            {
                layoutAid=toString((long long)start_file.getArticleByUrl(tempString).getIndex());
            }
            else
            {
                layoutAid="";
            }

            std::string tmp="";
            std::vector<std::string> rdlist;
            tempString=diff_file.getArticleByUrl("M/redirectlist").getPage();
            for(unsigned int i=0; i<tempString.size(); i++)
            {
                if(tempString[i]!='\n')
                {
                    tmp+=tempString[i];
                }
                else
                {
                    rdlist.push_back(tmp);
                    tmp="";
                }
            }
            for(unsigned i=0; i<rdlist.size(); i=i+2)
            {
                redirectList.push_back(std::make_pair(rdlist[i],rdlist[i+1]));
            }
            std::cout<<"\nProcessing redirect list..\n"<<std::flush;
            redirect.resize(start_file.getFileheader().getArticleCount()+diff_file.getFileheader().getArticleCount());
            for(unsigned int i=0; i<redirect.size(); i++)
            {
                redirect[i]=0;
            }
            //To fix
            for(unsigned int i=0; i<redirectList.size(); i++)
            {
                int destId;
                if(diff_file.getArticleByUrl(redirectList[i].second).getIndex()!=std::numeric_limits<unsigned int>::max())
                {
                    //If the article to which the redirect points is present n the diff_file.
                    destId=diff_file.getArticleByUrl(redirectList[i].second).getIndex()+start_file.getFileheader().getArticleCount();
                }
                else
                {
                    //If the article to which the redirect points to is not present in the diff_File, it HAS to be present in the start_file.
                    destId=start_file.getArticleByUrl(redirectList[i].second).getIndex();
                }
                int startId;

                if(diff_file.getArticleByUrl(redirectList[i].first).getIndex()!=std::numeric_limits<unsigned int>::max())
                {
                    //If the redirect was found in the diff_file.
                    startId=diff_file.getArticleByUrl(redirectList[i].first).getIndex()+start_file.getFileheader().getArticleCount();
                }
                else
                {
                    //If the article was not found in the diff_file- i.e. if it is in the start_file.
                    startId=start_file.getArticleByUrl(redirectList[i].first).getIndex();
                }
                //std::cout<<"\nstartID: "<<startId;
                //std::cout<<"\ndestID: "<<destId;
                //getchar();
                redirect[startId]=destId;
            }
            redirectList.clear();

            zim::Article a;
            for(zim::File::const_iterator it=diff_file.begin(); it!=diff_file.end(); ++it)
            {
                if(it->getLongUrl()=="M/dlist")
                {
                    a=*it;
                }
            }
            std::string dllist=a.getPage();
            tmp="";
            //Computing list of deleted articles and storing them in a vector.
            for(unsigned int i=0; i<dllist.size(); i++)
            {
                if(dllist[i]!='\n')
                    tmp+=dllist[i];
                else
                {
                    delete_list.push_back(tmp);
                    tmp="";
                }
            }

            //Process dlist.
            std::cout<<"\nProcessing Delete list..\n"<<std::flush;
            dlist.resize(start_file.getFileheader().getArticleCount()+diff_file.getFileheader().getArticleCount());
            for(unsigned int i=0; i<dlist.size(); i++)
            {
                dlist[i]=0;
            }
            for(unsigned int i=0; i<delete_list.size(); i++)
            {
                //Deleted articles will always be present in the start_file only.
                dlist[start_file.getArticleByUrl(delete_list[i]).getIndex()]=1;
            }
            delete_list.clear();
            return;
        }

        zim::Uuid zimPatch::ArticleSource::getUuid()
        {
            return fileUid;
        }
        std::string zimPatch::ArticleSource::getMainPage()
        {
            return mainAid;
        }
        std::string zimPatch::ArticleSource::getLayoutPage()
        {
            return layoutAid;
        }
        const zim::writer::Article* zimPatch::ArticleSource::getNextArticle()
        {

            //Add all articles in File_1 that have not ben deleted.
            std::string url="";

            if(index<start_file.getFileheader().getArticleCount()) //Meaning still on file_1
            {
                bool eof=false;     //To show wether EOF was reached or not.
                int id=0;
                zim::Article tmpAr=start_file.getArticle(index);
                id=index;
                while(dlist[index]==1)
                {
                    index++;
                    if(index>=start_file.getFileheader().getArticleCount())
                    {
                        eof=true;
                        break;
                    }
                    tmpAr=start_file.getArticle(index);
                    id=index;
                }
                if(!eof)
                {
                    tempArticle=Article(tmpAr,index);
                    id=index;
                    //If the article is also present in file_2
                    if(diff_file.getArticleByUrl(tmpAr.getLongUrl()).getIndex()!=std::numeric_limits<unsigned int>::max())
                    {
                        tmpAr=diff_file.getArticleByUrl(tmpAr.getLongUrl());
                        tempArticle=Article(tmpAr,start_file.getFileheader().getArticleCount()+diff_file.getArticleByUrl(tmpAr.getLongUrl()).getIndex());
                        id=tmpAr.getIndex()+start_file.getFileheader().getArticleCount();
                    }
                    index++;
                    if(redirect[id]!=0)
                        tempArticle.setRedirectAid(toString((long long)redirect[id]));
                    //std::cout<<"\nArticle: "<<tempArticle.getNamespace()<<"/"<<tempArticle.getUrl();
                    //std::cout<<"\nIndex: "<<tempArticle.getAid();
                    //getchar();
                    return &tempArticle;
                }
            }
            //Now adding new articles in file_2
            if(index<(start_file.getFileheader().getArticleCount()+diff_file.getFileheader().getArticleCount()))
            {
                int id=0;
                zim::Article tmpAr=diff_file.getArticle(index-start_file.getFileheader().getArticleCount());
                //If the article is already in file_1, it has been added.
                while(start_file.getArticleByUrl(tmpAr.getLongUrl()).getIndex()!=std::numeric_limits<unsigned int>::max()||isAdditionalMetadata(tmpAr.getLongUrl()))
                {
                    index++;
                    if(index>=(start_file.getFileheader().getArticleCount()+diff_file.getFileheader().getArticleCount()))
                        break;
                    tmpAr=diff_file.getArticle(index-start_file.getFileheader().getArticleCount());
                }
                if(index<(start_file.getFileheader().getArticleCount()+diff_file.getFileheader().getArticleCount()))
                {
                    tempArticle=Article(tmpAr,index);
                    id=index;
                    index++;
                    //std::cout<<"\nRedirectID: "<<redirect[id];
                    //std::cout<<"\nID: "<<id;
                    if(redirect[id]!=0)
                        tempArticle.setRedirectAid(toString((long long)redirect[id]));
                    //std::cout<<"\nArticle: "<<tempArticle.getNamespace()<<"/"<<tempArticle.getUrl();
                    //std::cout<<"\nIndex: "<<tempArticle.getAid();
                    //std::cout<<"\nIsredirect: "<<tempArticle.isRedirect();
                    //getchar();
                    return &tempArticle;
                }
            }
            return 0;
        }
        zim::Blob zimPatch::ArticleSource::getData(const std::string& aid)
        {
            unsigned int id=atoi(aid.c_str());
            if(id<start_file.getFileheader().getArticleCount())
            {
                return start_file.getArticle(id).getData();
            }
            if(id<(start_file.getFileheader().getArticleCount()+diff_file.getFileheader().getArticleCount()))
            {
                return diff_file.getArticle(id-start_file.getFileheader().getArticleCount()).getData();
            }
            std::cout<<"\nReturned null for: "<<aid;
            return zim::Blob("",0);
        }

}








