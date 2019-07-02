#include<string>
#include<vector>
#include<fstream>
#include<iostream>
#include<unordered_map>
#include<boost/algorithm/string.hpp>
#include<boost/filesystem/path.hpp>
#include<boost/filesystem/operations.hpp>
#include"../cppjieba/include/cppjieba/Jieba.hpp"

#include<algorithm>
#include<jsoncpp/json/json.h>
class FileUtil{
  public:
    static bool Read(const std::string&file_path,std::string* content)
    {
      std::ifstream file(file_path.c_str());
      if(!file.is_open()){
        return false;
      }
      //按行读取内容
      std::string line;
      while(std::getline(file,line)){
        *content += line + "\n";
      }

      file.close();
      return true; 
    }

    static bool Write(const std::string& file_path,const std::string& content)
    {
      std::ofstream file(file_path.c_str());
      if(!file.is_open()){
        return false;
      }
      file.write(content.c_str(),content.size());
      file.close();
      return true;
    }
};//end FileUtil 


class StringUtil{
  public:
    //基于boost的字符串切分来实现
    static void Split(const std::string&input,std::vector<std::string>* output,const std::string& split_char)
    {
      boost::split(*output,input,boost::is_any_of(split_char),boost::token_compress_off);
      //token_compress_off关闭压缩切分            aaa\3\3ccc    切分成3个字符串   
    }
};//end StringUtil 
