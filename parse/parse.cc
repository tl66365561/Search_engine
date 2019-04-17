#include"../common/util.hpp"

const std::string g_input="../data/input";
const std::string g_output="../data/output/raw_input";


struct DocInfo{
  std:: string title;
  std:: string content; 
  std:: string url;
};



bool Enum(const std::string& input,std::vector<std::string>* file_list){

namespace fs=boost::filesystem;
fs::path root_path(input);
if(!fs::exists(root_path)) {
    std::cout<<"input_path is not exist"<<std::endl;
    return false;

    }
//借助迭代器遍历目录
    fs::recursive_directory_iterator end_iter;
    for(fs::recursive_directory_iterator iter(root_path);iter!=end_iter;++iter){
    //剔除目录
    if(!fs::is_regular_file(* iter)){
        continue; 

    }
   //剔除其他类型的文件
    if(iter->path().extension()!=".html"){
        continue;
        }
     file_list->push_back(iter->path().string());
     }
     return true;
    
}

void write_output(DocInfo& doc_info,std::ofstream& file){

  std::string line=doc_info.title+'\3'+doc_info.url+'\3'+doc_info.content+'\n';
  //file<<line<<std::endl;
  file.write(line.c_str(),line.size());


}

bool ParseTitle(std::string& html,std::string* title){  //解析标题

size_t begin = html.find("<title>");
if(begin == std::string::npos){
      std::cout<<"<title> not fouund " << std::endl;
          return false;
            
}
  size_t end = html.find("</title>");
  if(end == std::string::npos){
        std::cout << "</title> not found" << std::endl;
            return false;
              
  }

  begin += std::string("<title>").size();
  if(begin > end){
        std::cout << "begin end error" << std::endl;
            return false;
              
  }
  *title = html.substr(begin,end-begin);
    return true;
}

bool ParseContent(std::string html,std::string *str){//解析正文
  
  bool flag=1;
  for(auto c:html){
    if(flag){

    if(c=='<')
    {
      flag=0;
    }
    else{
      //正文状态
      if(c=='\n')
        c=' ';

      str->push_back('c');


    }
    }
   
    else if(c=='>')
    {
      flag=1;
    }
 
     
    

  }
  return true;

} 

bool ParseUrl(std::string& file_path,std::string *str){

  std::string prefix = "//www.boost.org/doc/libs/1_53_0/doc/";
  std::string tail = file_path.substr(g_input.size());
  *str = prefix + tail;

  return true;


}





bool parse_file( std::string& file_path,DocInfo* doc_info){
 
  std::string html;
  bool ret=FileUtil::Read(file_path,&html);
  if(!ret){
    std::cout<<"read file error,file_path="<<file_path<<std::endl;
    return false;

  }
  //解析标题
  ret=ParseTitle(html,&doc_info->title);
  if(!ret){
    std::cout<<"parse title error,file_path="<<file_path<<std::endl;
    return false;
  }

  ret=ParseContent(html,&doc_info->content);
  if(!ret){
   std::cout<<"parse content error,file_path="<<file_path<<std::endl;
   return false;
  }
  ret=ParseUrl(file_path,&doc_info->url);
  if(!ret){
  std::cout<<"parse url error,file_path="<<file_path<<std::endl;

  }

return true;


}







int main()
{
  //先枚举出input目录下的所有文件路径
  std::vector<std::string>file_list;
  bool ret=Enum(g_input ,&file_list);
  if(!ret)
  {
    std::cout<<"Enum false !"<<std::endl;
    return 1;

  }

 std:: ofstream output_file(g_output.c_str());
 if(!output_file.is_open()){
   std::cout<<"open output error "<<" outputfile path=  "<<g_output<<std::endl;
   return 1;
 }

//去标签，添加到输出文件中

 for( std::string& file_path : file_list){
   DocInfo doc;
   ret=parse_file(file_path,&doc);
   
   if(!ret){
    std::cout<<"parse file error"<<std::endl;
    continue;

   }
   
   write_output(doc,output_file);


 }



  output_file.close();
  return 0;
}
