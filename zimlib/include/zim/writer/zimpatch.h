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
#include <zim/uuid.h>
#include <list>
#include <limits>
#include <algorithm>
#include <sstream>
#define toString( x ) dynamic_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()


namespace zim
{

class zimPatch
{
private:
    class Article : public zim::writer::Article         //Article clas that will be passed to the zimwriter. Contains a zim::Article class, so it is easier to add a
    {
        //article from an existing ZIM file.
        std::string _id;
        zim::Blob b;
        zim::Article Ar;
        bool isRd;
        std::string redirectAid;
    public:
        Article()
        {
            isRd=false;
        }
        explicit Article(const std::string& id)
        {
            isRd=false;
        }
        explicit Article(const zim::Article a,int id)
        {
            Ar=a;
            _id=toString((long long)id);
            b=Ar.getData();
            isRd=false;
        }
        void setRedirectAid(std::string a)
        {
            redirectAid=a;
            isRd=true;
            return;
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
            return isRd;
        }
        virtual std::string getMimeType() const
        {
            return Ar.getMimeType();
        }

        virtual std::string getRedirectAid() const
        {
            //return toString((long long)(Ar.getRedirectIndex()+1));
            return redirectAid;
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

    class ArticleSource : public zim::writer::ArticleSource
    {
        Article tempArticle;
        unsigned _next;
        zim::File start_file;
        zim::File diff_file;
        zim::Uuid fileUid;
        std::string mainAid;
        std::string layoutAid;
        zim::File::const_iterator it;
        std::vector<std::string> delete_list;
        std::vector< int > dlist;
        unsigned int index;
        unsigned int indexprev;
        std::vector< int> redirect;
        std::vector< std::pair<std::string , std::string > >redirectList;
        unsigned char val(char c)
        {
            if ('0' <= c && c <= '9') { return c      - '0'; }
            if ('a' <= c && c <= 'f') { return c + 10 - 'a'; }
            if ('A' <= c && c <= 'F') { return c + 10 - 'A'; }
            return '0';
        }
        char hexStringtoChar(std::string s)
        {
            if(s.size()!=2)
                return '0';
            unsigned char num1,num2;
            num1=val(s[0]);
            num2=val(s[1]);
            return num1*16+num2;
        }
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

    public:
        explicit ArticleSource(std::string,std::string);
        zim::Uuid getUuid();
        std::string getMainPage();
        std::string getLayoutPage();
        virtual const zim::writer::Article* getNextArticle();
        virtual zim::Blob getData(const std::string& aid);
    };

    ArticleSource src;
    zim::writer::ZimCreator c;
    std::string start_file;
    std::string end_file;
    std::string diff_file;

public:
    zimPatch(std::string filename_1,std::string filename_2,std::string filename_3,int argc, char* argv[])
        :
        src(filename_1,filename_2),
        c(argc,argv)
    {
        c.setMinChunkSize(2048);
        start_file=filename_1;
        diff_file=filename_2;
        end_file=filename_3;
    }
    void write()
    {
        c.create(end_file,src);
    }

};

}








