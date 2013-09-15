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
#include <list>
#include <algorithm>
#include <sstream>

#define toString( x ) dynamic_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

namespace zim
{

class zimDiff
{
private:

    class Article : public zim::writer::Article         //Article class that will be passed to the zimwriter. Contains a zim::Article class, so it is easier to add a
    {
        //article from an existing ZIM file.
        std::string _id;
        zim::Blob b;
        zim::Article Ar;
        bool isRd;
    public:
        Article()
        {
            isRd=false;
        }
        explicit Article(const std::string& id)
        {
            isRd=false;
        }
        explicit Article(const zim::Article a)
        {
            Ar=a;
            _id=toString((long long)Ar.getIndex());
            b=Ar.getData();
            isRd=false;
        }
        void setRedirect()
        {
            isRd=true;
        }
        virtual std::string getAid() const
        {
            return _id;
        }
        virtual char getNamespace() const
        {
            return Ar.getNamespace();
        }

        virtual std::string getUrl() const
        {
            return Ar.getUrl();
        }
        virtual std::string getTitle() const
        {
            return Ar.getTitle();
        }
        virtual bool isRedirect() const
        {
            return false;
        }
        virtual std::string getMimeType() const
        {
            if(isRd)
            {
                return "redirect";
            }
            else
            {
                return Ar.getMimeType();
            }
        }

        virtual std::string getRedirectAid() const
        {
            //return std::to_string((long long)Ar.getRedirectIndex()+1);
            return "";
        }

        virtual std::string getParameter()
        {
            return Ar.getParameter();
        }
        zim::Blob data()
        {
            return b;
        }
    };


    class ArticleRaw : public zim::writer::Article  //Article class, stores all data in itself. Easier for adding new articles.
    {
    public:
        std::string _id;
        std::string _data;
        std::string Url;
        std::string Title;
        std::string MimeType;
        std::string RedirectAid;
        char nm;
        bool IsRedirect;
        ArticleRaw()  { }
        virtual std::string getAid() const
        {
            return _id;
        }
        virtual char getNamespace() const
        {
            return nm;
        }
        virtual std::string getUrl() const
        {
            return Url;
        }
        virtual std::string getTitle() const
        {
            return Title;
        }
        virtual bool isRedirect() const
        {
            return IsRedirect;
        }
        virtual std::string getMimeType() const
        {
            return MimeType;
        }
        virtual std::string getRedirectAid() const
        {
            return RedirectAid;
        }
        zim::Blob data()
        {
            return zim::Blob(&_data[0], _data.size());
        }
    };


    class ArticleSource : public zim::writer::ArticleSource
    {
        Article tempArticle;
        ArticleRaw dlist;               //Metadata article containing list of articles to be deleted.
        ArticleRaw startFileUID;        //Metadata article containing start file UID
        ArticleRaw endFileUID;          //Metadata article containing end file UID
        ArticleRaw mainAurl;
        ArticleRaw layoutAurl;
        ArticleRaw redirectList;
        int fileSize;
        zim::File file_1;
        zim::File file_2;
        zim::File::const_iterator itr;
        std::list<std::string > deleteList;
        bool dlistToggle;
        bool startIdToggle;
        bool endIdToggle;
        bool mainAurlToggle;
        bool layoutAurlToggle;
        bool redirectListToggle;
    public:
        explicit ArticleSource(std::string ,std::string );
        virtual const zim::writer::Article* getNextArticle();
        virtual zim::Blob getData(const std::string& aid);
    }src;
    zim::writer::ZimCreator c;
    std::string start_file;
    std::string end_file;
    std::string diff_file;
public:
    zimDiff(std::string filename_1,std::string filename_2,std::string filename_3,int argc,char *argv[])
:
    src(filename_1,filename_2),
    c(argc,argv)
    {
        start_file=filename_1;
        end_file=filename_2;
        diff_file=filename_3;

    }
    void write()
    {
        c.create(diff_file, src);
    }
};

}





















