#include "StringUtils.h"
#include <algorithm> 
#include <cctype>
#include <cstdio>
#include <string>
#include <cmath>

namespace StringUtils{

std::string Slice(const std::string &str, ssize_t start, ssize_t end){
    //Your code goes here
    std::string result;
    if(start>= 0 && end >= 0){

    if(end == 0){
        if(start > str.length()){
            result = "";
            return result;
        }
        else{
            result = str.substr(start);
            return result;
        }
    }

    else if(start > str.length() || end > str.length()){
        result = "";
        return result;
    }

    else{
        result = str.substr(start,end-start);
        return result;
    }
}

else if(start == 0){
    result = str.substr(0,str.length() + end);
    return result;
}

else if(start > 0){
    result = str.substr(start,str.length() + end - start);
    return result;
}

else{
    result = str.substr(str.length() + start, abs(start - end));
    return result;
}
}

std::string Capitalize(const std::string &str){
    // Your code goes here
    std::string lowercase;
    for (char ch:str){
        lowercase += tolower(ch);
    }
    lowercase[0] = toupper(lowercase[0]);
    return lowercase;
}

std::string Title(const std::string &str){
    // Your code goes here
    std::string lowercase;
    for(int i = 0; i < str.length(); i++){
        lowercase += tolower(str[i]);
        if(i == 0 || str[i-1] == ' '|| str[i-1] == '-' || str[i-1] == '\'' || str[i-1] == '0' || str[i-1] == '1'
        || str[i-1] == '2'|| str[i-1] == '3'|| str[i-1] == '4'|| str[i-1] == '5'|| str[i-1] == '6'|| str[i-1] == '7'|| str[i-1] == '8'
        || str[i-1] == '9'){
                lowercase[i] = toupper(str[i]);
    }
}
    return lowercase;
}

std::string LStrip(const std::string &str){
    // Your code goes here
    std::string nolspace;
    int pos = 0;
    for(int i = 0; i < str.length(); i ++){
        if (str[i] != ' '){
            break;
        }
        else{
            pos++;
        }
    }

    for(int i = pos; i < str.length(); i++){
        nolspace += str[i];
    }
    return nolspace;
}

std::string RStrip(const std::string &str){
    // Your code goes here
    std::string noRspace;
    int pos = str.length();
    for(int i = str.length() -1 ; i >= 0; --i){
        if (str[i] != ' '){
            break;
        }
        else{
            pos--;
        }
    }

    for(int i = 0; i < pos; i++){
        noRspace += str[i];
    }
    return noRspace;
}

std::string Strip(const std::string &str){
    // Your code goes here
    std::string nospace;
    nospace = RStrip(LStrip(str));

    return nospace;
}

std::string Center(const std::string &str, int width, char fill){
    // Your code goes here
    std::string centered;
    int right = (width - str.length()) / 2;
    int left = (width - str.length()) / 2;

    if((width - str.length()) % 2 != 0){
        right = right + 1;
    }

    for(int i = 0; i < left; i++){
        centered += fill;
    }

    for(int i = 0; i < str.length(); i++){
        centered += str[i];
    }
    
    for(int i = 0; i < right; i++){
        centered += fill;
    }

    return centered;
}

std::string LJust(const std::string &str, int width, char fill){
    // Your code goes here
    std::string centered;
    int Right = width - str.length();

    for(int i = 0; i < str.length(); i++){
        centered += str[i];
    }
    for(int i = 0; i < Right; i++){
        centered += fill;
    }
    return centered;
}

std::string RJust(const std::string &str, int width, char fill){
    // Your code goes here
    std::string centered;
    int Left = width - str.length();

    for(int i = 0; i < Left; i++){
        centered += fill;
    }

    for(int i = 0; i < str.length(); i++){
        centered += str[i];
    }
    return centered;
}

std::string Replace(const std::string &str, const std::string &old, const std::string &rep){
    // Your code goes here
    std::string replaced;
    std::string sub;
    int pos = 0;
    sub = str;
    replaced = "";

    while(sub.find(old) != -1){
        pos = sub.find(old);
        replaced += sub.substr(0,pos);
        replaced += rep;
        sub = str.substr(replaced.length(),str.length());

    }
    if(replaced.length() != str.length()){
        replaced += str.substr(replaced.length(),str.length());
    }
    
    return replaced;
}

std::vector< std::string > Split(const std::string &str, const std::string &splt){
    //Your code goes here
    std::vector<std::string> spltlist;
    std::string replaced;
    std::string sub;
    sub = str;
    int counter = 0;
    int pos = 0;

    if(splt.empty() || splt == " "){
        for(int i = 0; i < str.length(); i++){
            if(str[i] == ' ' || str[i] == '\t'|| str[i] == '\n'){
                if(replaced == ""){
                    counter++;
                }
                else{
                    spltlist.push_back(replaced);
                    replaced = "";
                    counter++;
                }
            }
            else{
                replaced += str[i];
                counter++;
            }
        }
        if(counter == str.length()){
            spltlist.push_back(replaced);
        }
        return spltlist;
    }

    else{
        while(sub.find(splt) != -1){
            pos = sub.find(splt);
            replaced += sub.substr(0,pos);
            replaced += splt;
            spltlist.push_back(sub.substr(0, pos));
            sub = str.substr(replaced.length(),str.length());
        }   

        sub = str.substr(replaced.length(),str.length());

        if(replaced.length() != str.length()){
            spltlist.push_back(sub);
        }
        return spltlist;
    }
}

std::string Join(const std::string &str, const std::vector< std::string > &vect){
    // Your code goes here
    std::string joined;
    for(int i= 0; i < vect.size() - 1;i++){
        joined += vect[i];
        joined += str;
    }
    joined += vect[vect.size()-1];

    return joined;
}

std::string ExpandTabs(const std::string &str, int tabsize){
    // Your code goes here
    std::string tabstr;
    std::string sub;
    std::string sub2;
    std::string replaced;
    int counter = 1;
    int ws = 0;
    int co = 1;
    int tabstop = tabsize;
    int pos = 0;
    sub = str;
    sub2 = str;

    while(sub.find('\t') != -1){
        if(tabsize ==0){
            while(sub.find('\t') != -1){
                pos = sub.find('\t');
                replaced += sub.substr(0,pos);
                replaced += "";
                sub = str.substr(replaced.length() + co,str.length());
                co++;

        }
        if(replaced.length() + co - 1!= str.length()){
            replaced += str.substr(replaced.length() + co - 1,str.length());
        }
    
        return replaced;
        }
    
        pos = sub.find('\t');
        replaced += sub.substr(0, pos);
        sub = str.substr(replaced.length() + co, str.length());
            
        if(pos >= tabstop){
            while(pos >= tabstop){
                tabstop = tabsize * counter;
                counter++;
            }
        }
        else if(pos < tabstop){
    
        }
        else{
            tabstop = tabsize * counter;
            counter++;
        }

        ws = tabstop - pos;

        tabstr += sub2.substr(0,pos);
        sub2 = str.substr(replaced.length() + co, str.length());
        for(int i = 0; i < ws; i++){
            tabstr += " ";
        }
        co++;
        counter = 1;
        tabstop = tabsize;
    }

    if(replaced.length() + co - 1 != str.length()){
        tabstr += str.substr(replaced.length() + co - 1,str.length());
    }

    return tabstr;
}

int EditDistance(const std::string &left, const std::string &right, bool ignorecase){
    // Your code goes here
    std::vector<std::string> sup;
    std::vector<std::string> sup2;
    std::string p1;
    std::string p2;
    int counter = 0;
    int dif = 0;
    int min = 0;
    int max = 0;
    sup = Split(left);
    sup2 = Split(right);

    if(ignorecase == true){
        return 0;
    }

    if(sup.size() == 1 || sup2.size() == 1){
        if(left.length() == right.length()){
            for(int i =0; i<left.length();i++){
                if(left[i] != right[i]){
                    counter++;
                }
            }
            return counter;
        }

        else{
            if(left.length() > right.length()){
                min = right.length();
                max = left.length();
            }
            else{
                min = left.length();
                max = right.length();
            }

            for(int i =0; i<min;i++){
                if(left[i] == right[i]){
                    counter++;
                }
            }

            //dif = abs(right.length() - left.length()) - counter;
            dif = max - counter;
            return dif;
        }
    }

    else{
        for(int i = 0; i<sup.size(); i++){
            p1 = sup[i];
            p2 = sup2[i];
            min = 0;
            max = 0;
            counter = 0;

            if(p1 == p2){
            }

            else if(p1.length() == p2.length()){
                    for(int k =0; k<p1.length();k++){
                        if(p1[k] != p2[k]){
                            counter++;
                        }
                    }
                dif += counter;
                }

            else{
                if(p1.length() > p2.length()){
                    min = p2.length();
                    max = p1.length();
                }

                else{
                    min = p1.length();
                    max = p2.length();
                }

                for(int l =0; l<min;l++){
                    if(p1[l] == p2[l]){
                        counter++;
                    }
                }

                dif += max - counter;
                
            }
        }
    return dif;
    }
}
}
