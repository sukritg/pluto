#include "reader.h"

reader::reader(std::string _fileName)
{
    std::ifstream inFile(_fileName);
    if (inFile.is_open())
    {
        std::cout << "Input file = " << _fileName << std::endl;
        std::string line, tok;
        while (std::getline(inFile,line,'\n'))
        {
            // Skip comments in input file
            tok.clear();
            for (char i:line)
            {
                tok += i;
                if (tok == " ")
                    tok.clear();

            }

            if (tok[0] == '#' || tok == "\r")
            {

                tok.clear();
                continue;
            }
            else
                fileContents += line ;
        }
        inFile.close();

        //for (auto i:fileContents)
        //   std::cout << i << std::endl;
    }

    else
        std::cout << "File not found." << std::endl;
}

reader::~reader()
{

}


void reader::configKeys()
{
    std::string _fileName = "keyConfig.dat";
    std::ifstream inFile(_fileName);
    if (inFile.is_open())
    {
        std::cout << "Configuring reserved keywords from " << _fileName << std::endl;
        std::string line,tok;
        while (std::getline(inFile,line))
        {
            for (auto i:line)
            {
                if (i != ' ')
                    tok += i;
                if (i == '\r')
                    tok.erase(tok.end()-1);


            }
            if (tok[0] == '#')
            {
                tok.clear();
                continue;
            }
                if (tok.empty() == false && tok[0] == '$')
                reservedKeys.push_back(tok);

            tok.clear();
        }

        inFile.close();
    }
    else
        std::cout << "Keyword config file not found." << std::endl;
}


void reader::printKeys()
{
      for (auto i:reservedKeys)
            std::cout << i << std::endl;
}

void reader::lexer()
{
    std::string tokenID;
    bool state = false;
     bool sflag = false;
    bool mflag = false;
    std::string tok = "";

    for (char i:fileContents)
    {
        tok += i;
        if (tok == " ")
            tok.clear();
        else if ((i == ' ') && (checkItem<std::string>(reservedKeys,tok.substr(0,tok.length()-1)) == true))
        {
            //std::cout << "Token found : " << tok;
            tokenID = tok.substr(0,tok.length()-1);
            state = true;
            tok.clear();
        }
        else if (i == ';' && state == true)
        {
            tok.erase(tok.end()-1);
            std::string str;
            str = "";
            for (auto s:tok)
            {
                str +=s;
                if (str == " ")
                    {str = ""; }
                else if (s == '=')
                    {str = ""; sflag = true; mflag = false;}
                else if (s == '{')
                    {str = ""; sflag = false; mflag = true;}
            }


            if (sflag == true && tokenID != "$PROGRAM")
            {
                std::vector<double> value;
                value.push_back(toDouble(str));
                tokenList[tokenID] = value;
                value.clear();
                str.empty();
                sflag = false;
            }

            if (mflag == true && tokenID != "$PROGRAM")
            {
                std::vector<double> value;
                std::string p;
                for (auto v:str)
                {
                    p += v;
                    if (p == " ")
                        p.clear();
                    else if (v == ',' || v == '}')
                        {
                            p.erase(p.end()-1);
                            value.push_back(toDouble(p));
                            p.clear();
                        }

                }
                tokenList[tokenID] = value;
                value.clear();
                str.empty();
                mflag = false;

            }

            if (tokenID == "$PROGRAM")
            {
                for (auto c = str.end(); c >= str.begin(); c--)
                {
                    if (*c == ' ')
                    {
                       str.erase(std::distance(str.begin(), c));
                    }
                       }
                moduleName = str;
                std::cout << moduleName <<std::endl;
                str.empty();
                sflag = false; mflag = false;
            }
            //std::cout << " : " << tok <<std::endl;
            tok.clear();
            state = false;
        }
        else if (i == ';' && state == false)
        {
            tok.clear();
        }
        else if (i == '\r')
            tok.erase(tok.end()-1);

        }
    }

void reader::printTokenList()
{
    std::cout << "--------------------------------------------------" <<std::endl;
    std::cout << "Module - " << moduleName << std::endl;
    std::cout << "Token List Map" <<std::endl;
    std::cout << "--------------------------------------------------" <<std::endl;

    for (auto i : tokenList)
    {

            std::cout << i.first ;
            for (auto j : i.second)
                std::cout << " " << j ;
        std::cout << std::endl;
    }
}


 std::map<std::string,std::vector<double>> reader::getTokens()
 {
     return tokenList;
 }
std::string reader::getModuleName()
{
    return moduleName;
}
template <class V>
bool reader::checkItem(std::vector<V> _list,V _item)
{

    //std::cout << "Check : " << _item << std::endl;
    for (auto i:_list)
    {

        if (i == _item)
            return true;

    }
    return false;
}
