#include "Mylib.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <map>
#include <vector>
#include <algorithm>
#include <set>

using namespace std;

bool IsWordCharacter(char c)
{
    return isalpha(c) || c == '_';
}

void Read(map<string, int> &wordCounts, vector<string> &URLs, stringstream &buf, map<string, set<int>> &wordLines)
{
    string line;
    int lineNumber = 1;
    
    while (getline(buf, line))
    {
        stringstream lineStream(line);
        string word;
        
        while (lineStream >> word)
        {
            // Remove symbols and numbers from the beginning and end of the word
            word.erase(word.begin(), find_if(word.begin(), word.end(), IsWordCharacter));
            word.erase(find_if(word.rbegin(), word.rend(), IsWordCharacter).base(), word.end());

            if (!word.empty())
            {
                // Process the word and update the wordCounts map accordingly
                ++wordCounts[word];
                wordLines[word].insert(lineNumber);
            }
        }
        
        ++lineNumber;
    }
    
    string text = buf.str();
    regex urlRegex(R"((?:https?://)?(?:www\.)?([a-zA-Z0-9]+\.)+[a-zA-Z]{2,3}(?:\.[a-zA-Z]{2})?)");
    sregex_iterator it(text.begin(), text.end(), urlRegex);
    sregex_iterator end;
    
    map<string, int> urlCounts;
    
    while (it != end)
    {
        string url = it->str();
        ++urlCounts[url];
        ++it;
    }
    
    for (const auto &entry : urlCounts)
    {
        string url = entry.first;
        int count = entry.second;
        
        if (count > 1)
        {
            url += " (Repeated: " + to_string(count) + ")";
        }
        
        URLs.push_back(url);
    }
}

void WriteWordCounts(const map<string, int> &wordCounts, const string &outputFile)
{
    ofstream output(outputFile);
    if (!output)
    {
        cerr << "Failed to open output file: " << outputFile << endl;
        return;
    }
    
    for (const auto &entry : wordCounts)
    {
        if (entry.second > 1)
        {
            output << entry.first << ": " << entry.second << endl;
        }
    }
    
    output.close();
}

void WriteURLs(const vector<string> &URLs, const string &outputFile)
{
    ofstream output(outputFile);
    if (!output)
    {
        cerr << "Failed to open output file: " << outputFile << endl;
        return;
    }
    
    for (const auto &url : URLs)
    {
        output << url << endl;
    }
    
    output.close();
}

void WriteCrossReference(const map<string, set<int>> &wordLines, const string &outputFile)
{
    ofstream output(outputFile);
    if (!output)
    {
        cerr << "Failed to open output file: " << outputFile << endl;
        return;
    }
    
    for (const auto &entry : wordLines)
    {
        const string &word = entry.first;
        const set<int> &lines = entry.second;
        
        if (lines.size() > 1)
        {
            output << word << ":";
            
            for (int line : lines)
            {
                output << " " << line;
            }
            
            output << endl;
        }
    }
    
    output.close();
}

int main()
{
    string inputFile = "text.txt";
    string wordCountsOutputFile = "word_counts.txt";
    string URLsOutputFile = "urls.txt";
    string crossReferenceOutputFile = "cross_reference.txt";
    
    ifstream input(inputFile);
    if (!input)
    {
        cerr << "Failed to open input file: " << inputFile << endl;
        return 1;
    }
    
    stringstream buf;
    buf << input.rdbuf();
    input.close();
    
    map<string, int> wordCounts;
    vector<string> URLs;
    map<string, set<int>> wordLines;
    
    Read(wordCounts, URLs, buf, wordLines);
    
    WriteWordCounts(wordCounts, wordCountsOutputFile);
    WriteURLs(URLs, URLsOutputFile);
    WriteCrossReference(wordLines, crossReferenceOutputFile);

    cout << "Check new files: " << endl;
    cout << endl;
    cout << setw(20) << left << "word_counts.txt " << " | " << setw(10) << "List of every word that repeated itself more than once + counter" << endl;
    cout << setw(20) << left << "cross_reference.txt " << " | " << setw(10) << "Cross reference of every word" << endl;
    cout << setw(20) << left << "urls.txt " << " | " << setw(10) << "List of URLs from the input text" << endl;
    cout << endl;
    
    return 0;
}
