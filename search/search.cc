
#include"search.h"


namespace searcher{




//在内存中构建索引
  bool index::build(std::string input_path)const{
    std:: ifstream file(input_path);
    if(!file.is_open()){
     std::cout<<"file_path open error,file_path="<<input_path<<std::endl;
     return false;
  
    }
    
    std::string line;
    while(std::getline(file,line)){
//1.对文件内容进行解析，获取doc_info对象，插入正排索引中;
   const DocInfo* docinfo =  build_forward_index(line);

//2.提取doc_info对象，更新倒排索引
   build_inverted_index(*docinfo);
   if(docinfo->doc_id%500==0){
     std::cout<<"doc_id== "<<docinfo->doc_id<<std::endl;

   }

    }

file.close();
return true;
  }


//构建正排索引
  Docinfo* index::  build_forward_index(const std::string& line){
 
   std::vector<std::string>tokens;
   StringUtil::Split(line,&tokens,"\3");
   if(tokens.size()!=3)
   {
    std::cout<<"split error "<<std::endl;
    return NULL;

   }

   //构建DocInfo对象
   DocInfo docinfo;
//观察doc_id的作用
   docinfo.id=_forward_index.size();
   docinfo.title=tokens[0];
   docinfo.content=tokens[1];
   docinfo.url=tokens[2];
   _forward_index.push_back(docinfo);

   return &_forward_index.back();


  }


  //更新倒排索引
  void index:: build_inverted_index(const DocInfo& doc){
//1.分词
    std::vector<std::string>title_tokens;
    CutWord(doc.title,&title_tokens);
    std::vector<std::string>content_tokens;
    CutWord(doc.content,&content_tokens);

//2.词频统计 用一个hash表完成词频统计
    struct WordCnt{
      int title_cnt;
      int content_cnt;

    }
 
    std::unordermap<std::string,WordCnt>word_cnt;
 
 for(auto word:title_tokens){
   boost::to_lower(word);
   ++word_cnt[word].title_cnt;
  
 }
 for(auto word:content_tokens){
   boost::to_lower(word);
   ++word_cnt[word].content_cnt;

 }


//3.遍历分词结果，在倒排索引中查找
 for(const auto& word_pair:word_cnt){
        Weight weight;
        weight.doc_id = doc_info.doc_id;
        weight.weight = 10*word_pair.second.title_cnt + word_pair.second.content_cnt;
        //观察 weight 中key的作用
        weight.key = word_pair.first;

 }

//4.在倒排索引中插入weight对象
      inverted_list& invertedlist = _inverted_index_[word_pair.first];
      invertedlist.push_back(weight); 


  }

  void CutWord(const std::string input,std::vector<std::string>* output){

    jieba_.CutForSearch(input,*output);


  }









}//end searcher



