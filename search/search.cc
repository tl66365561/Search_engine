#include"search.h"

namespace search{
  const char* const DICT_PATH = "../cppjieba/dict/jieba.dict.utf8";
  const char* const HMM_PATH = "../cppjieba/dict/hmm_model.utf8";
  const char* const USER_DICT_PATH = "../cppjieba/dict/user.dict.utf8";
  const char* const IDF_PATH = "../cppjieba/dict/idf.utf8";
  const char* const STOP_WORD_PATH = "../cppjieba/dict/stop_words.utf8";

  Index::Index():jieba_(DICT_PATH,HMM_PATH,USER_DICT_PATH,IDF_PATH,STOP_WORD_PATH){}
  bool Index::Build(const std::string& input_path)
  {
    std::cout << "Index Build Start!!!" << std::endl;
    //1.按行读取文件内容
    std::ifstream file(input_path.c_str());
    if(!file.is_open()){
      std::cout << "input_path open failed input_path= " << input_path << std::endl;
      return false;
    }
    std::string line;
    while(std::getline(file,line)){
    //2.对文件内容解析，构造DocInfo对象，插入forward_index数组，构建正排索引
    const Doc_Info* doc_info = BuildForward(line);
    //3.更新构建倒排索引
    BuildInverted(*doc_info);
    if(doc_info->doc_id%500==0)
      std::cout << "Build doc_id= " << doc_info->doc_id << std::endl;
    }
    std::cout << "Index Build Finish!!!" << std::endl;
    file.close();
    return true;
  }

  const Doc_Info* Index::GetDocInfo(uint64_t doc_id) const//正排索引
  {
    if(doc_id >= forward_index_.size()){
      return NULL;
    }
    return &forward_index_[doc_id];
  }

  const InvertedList* Index::GetInvertedList(const std::string& key) const//倒排索引，得到倒排拉链
  {
    auto pos = inverted_index_.find(key);
    if(pos == inverted_index_.end()){
      return NULL;
    }
    return &pos->second;
  }

  const Doc_Info* Index::BuildForward(const std::string& line){

    std::vector<std::string> tokens;//存放切分结果

    StringUtil::Split(line,&tokens,"\3");
    if(tokens.size()!=3){
      std::cout << "tokens not ok" << std::endl;
      return NULL;
    }
    //构造Doc_Info对象
    Doc_Info doc_info;
    doc_info.doc_id = forward_index_.size();
    doc_info.title = tokens[0];
    doc_info.url = tokens[1];
    doc_info.content = tokens[2];

    //将Doc_Info对象插入到forward_index_数组中
    forward_index_.push_back(doc_info);
    return &forward_index_.back();
  }

  //构建倒排拉链
  void Index::BuildInverted(const Doc_Info& doc_info)
  {
    //1.分词
    std::vector<std::string> title_tokens;
    CutWord(doc_info.title,&title_tokens);
    std::vector<std::string> content_tokens;
    CutWord(doc_info.content,&content_tokens);
    //2.词频统计
    struct WordCnt{
      int title_cnt;
      int content_cnt;
    };
    //用一个hash表完成词频统计
    std::unordered_map<std::string,WordCnt> word_cnt;
    for(std::string word:title_tokens){
      boost::to_lower(word);//统计词频应该忽略大小写，全转换成小写
      ++word_cnt[word].title_cnt;
    }
    for(std::string word:content_tokens){
      boost::to_lower(word);
      ++word_cnt[word].content_cnt;
    }
    //3.遍历分词结果，在倒排索引中查找
    for(const auto& word_pair:word_cnt){
      Weight weight;
      weight.doc_id = doc_info.doc_id;
      weight.weight = 10*word_pair.second.title_cnt + word_pair.second.content_cnt;
      weight.key = word_pair.first;

      //4.在倒排索引中插入Weight对象
      InvertedList& inverted_list = inverted_index_[word_pair.first];
      inverted_list.push_back(weight); 
    }
  }//end BuildInverted 

  void Index::CutWord(const std::string&input,std::vector<std::string>* output)
  {
    jieba_.CutForSearch(input,*output);
  }


  //搜索实现
  bool Search::Init(const std::string& input_path)
  {
    return index_->Build(input_path);
  }

  bool Search::Searchs(const std::string& query,std::string* json_result)
  {
    //1.对查询词进行分词
    std::vector<std::string> tokens;
    index_->CutWord(query,&tokens);

    //2.触发，针对分词结果查倒排索引
    std::vector<Weight> all_tokens_result;
    for(std::string word : tokens){
      boost::to_lower(word);
      auto* inverted_list = index_->GetInvertedList(word);
      if(inverted_list == NULL){
        continue;
      }
      all_tokens_result.insert(all_tokens_result.end(),inverted_list->begin(),inverted_list->end());
      //将得到的inverted_list数组整体尾插到all_tokens_result数组中
      //合并有序链表，去除重复项
    }
    //3.排序
    std::sort(all_tokens_result.begin(),all_tokens_result.end(),[](const Weight& w1,const Weight& w2){
        return w1.weight > w2.weight;
        });
    //4.构造结果，查正排，找到每个搜索结果的标题、正文、url
    //[
    //    {
    //        "title" = "标题",
    //        "desc" = "描述",
    //        "url" = "带a标签的url"
    //    },
    //]
    Json::Value results;//所有结果
    for(const auto& weight : all_tokens_result){
      const auto* doc_info  = index_->GetDocInfo(weight.doc_id);
      if(doc_info == NULL){
        continue;
      }
      //利用JSON结构
      Json::Value result;//单个结果
      result["title"] = doc_info->title;
      result["url"] = doc_info->url;
      result["desc"] = GetDesc(doc_info->content,weight.key);

      results.append(result);
    }
    Json::FastWriter writer;//借助Jsoncpp能够快速完成序列化
    *json_result = writer.write(results);
    return true;
  }//end Searchs函数

  //生成描述信息
  std::string Search::GetDesc(const std::string& content,const std::string&key)
  {
    //查找key的位置
    size_t pos = content.find(key);
    if(pos == std::string::npos){
      //该词只在标题中出现
      if(content.size() < 100)
        return content;
      else{
        return content.substr(0,100);
      }
    }

    //key在正文中，截取key前50词和key后50词
    size_t start = pos < 50 ? 0 : pos-50;
    if(start + 100 >=content.size()){
      return content.substr(start);
    }else{
      return content.substr(start,100);
    }
  }
};//end search
