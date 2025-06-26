#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <limits>
#include <algorithm>
#include <windows.h>

using namespace std;

class Node // creating a class node for store dictionary in trie data structure
{
public:
    map<char, Node *> child; // using map for store a node efficiently store child node for each character
    bool endOfWord;          // endofword is a flag which show word is end or not.

    Node() : endOfWord(false) {}
};

class SpellChecker // implements a method for spell checking
{
private:
    Node *rootNode;

public:
    SpellChecker() // constructor
    {
        rootNode = new Node(); //
    }

    int computeEditDistance(const string &s1, const string &s2) // for compute  distance between two strings
    {
        int len1 = s1.size();
        int len2 = s2.size();

        if (len1 == 0)
            return len2;
        if (len2 == 0)
            return len1;

        vector<vector<int>> vec(len1 + 1, vector<int>(len2 + 1, 0));

        for (int i = 0; i <= len1; ++i)
        {
            vec[i][0] = i;
        }

        for (int j = 0; j <= len2; ++j)
        {
            vec[0][j] = j;
        }

        for (int i = 1; i <= len1; ++i)
        {
            for (int j = 1; j <= len2; ++j)
            {
                if (s1[i - 1] == s2[j - 1])
                {
                    vec[i][j] = vec[i - 1][j - 1];
                }
                else
                {
                    vec[i][j] = 1 +  min({vec[i - 1][j], vec[i][j - 1], vec[i - 1][j - 1]});
                }
            }
        }

        return vec[len1][len2];
    }

    // this function find all sugestion for the misspelled word based on most similiar word

    void findSuggestions(Node *currNode, const string &str, const string &target, vector<string> &suggestions, int maxDist, int &minDist)
    {
        if (currNode->endOfWord)
        {
            int distance = computeEditDistance(str, target);
            if (distance <= maxDist && distance < minDist)
            {
                minDist = distance;
                suggestions.clear();
                suggestions.push_back(str);
            }
            else if (distance <= maxDist && distance == minDist)
            {
                suggestions.push_back(str);
            }
        }

        for (auto it = currNode->child.begin(); it != currNode->child.end(); ++it)
        {
            char nextChar = it->first;
            Node *nextNode = it->second;
            if (nextNode != nullptr)
            {
                findSuggestions(nextNode, str + nextChar, target, suggestions, maxDist, minDist);
            }
        }
    }

    // this function for add a word in dictionary ( custom dictionary  if user want)

    void addWord(const string &word)
    {
        Node *temp = rootNode;
        for (char c : word)
        {
            if (!temp->child.count(c))
            {
                temp->child[c] = new Node();
            }
            temp = temp->child[c];
        }
        temp->endOfWord = true;
    }

    // function for remove the word from dictionary  ( custom dictionary  if user want)

    void removeWordFromDictionary(const string &word)
    {
        if (!searchWord(word))
        {
            cout << "This word doesn't exist in the dictionary." << endl;
        }
        else
        {
            removeWordUtil(rootNode, word, 0);
        }
    }

    void removeWordUtil(Node *current, const string &word, int depth)
    {
        if (!current)
            return;

        if (depth == word.length())
        {
            current->endOfWord = false;
            return;
        }

        char ch = word[depth];
        if (current->child.find(ch) != current->child.end())
        {
            Node *nextNode = current->child[ch];
            removeWordUtil(nextNode, word, depth + 1);

            if (nextNode->child.empty() && !nextNode->endOfWord)
            {
                current->child.erase(ch);
                delete nextNode;
            }
        }
    }

    // function for search a word in dictionary useful function that check the given spelling is correct or not

    // if word not found in dictionary that means the given spelling is wrong

    // if given spelling is correct and not exist in dictionary than use custom dictionary
    bool searchWord(const string &word)
    {
        Node *temp = rootNode;
        for (char c : word)
        {
            if (temp->child.count(c) == 0)
            {
                return false;
            }
            temp = temp->child[c];
        }
        return temp->endOfWord;
    }

    vector<string> getWordSuggestions(const string &word, int maxDist = 1)
    {
        vector<string> suggestions;
        int minDist = numeric_limits<int>::max();

        findSuggestions(rootNode, "", word, suggestions, maxDist, minDist);

        return suggestions;
    }
};
// function is use for remove fullstop or coma at the end of the word that reduce the time for checking and replace the wrong spelling with corrrect one
string removespecialchar(string s)
{
    string result = "";
    for (int i = 0; i < s.length(); i++)
    {
        if (s[i] != '.' && s[i] != ',' && s[i] != '!')
        {
            result += s[i];
        }
    }
    return result;
}

vector<string> split(const string &input) // this function is used for store a individual store a suggestion into the vector (type vector)
{
    vector<string> words;
    string word;
    for (char c : input)
    {
        if (c == ' ')
        {
            if (!word.empty())
            {
                words.push_back(word);
                word.clear();
            }
        }
        else
        {
            word += c;
        }
    }
    if (!word.empty())
    {
        words.push_back(word);
    }
    return words;
}

int main()
{
    SpellChecker spl; // creating a object spl of class spellchecker
    string line, word;
    int wordcount = 0; // declareing a integer type variable to count the word from input file

    ifstream dictFile("word.txt"); // open the dictionary file in read mode
    if (dictFile.is_open())        // when file is open
    {
        while (getline(dictFile, line)) // for reading each line from the dictionary
        {
            spl.addWord(line); // adding each word in the dictionary
        }
        dictFile.close(); // clode the dictionary file
    }
    else
    {
        cout << "Error: Unable to open dictionary file." << endl; // error occurs when file can not open
        return 1;
    }
    int condition; // asking user for the enter a word to perform the opration what they need
    cout << "please enter your text file name here :";
    string filename; // enter the text document name which user want check spelling mistake
    cin >> filename;
    do
    {
        //--------------------------------------------printing all condition------------------------------------------------------------//

        cout << endl;
        cout << "************************************************************************************************************************************** \n"
             << endl;
        cout << "******-----> enter 0 to exit the loop \n " << endl;
        cout << "******-----> enter 1 to add a word in dictionary \n " << endl;
        cout << "******-----> enter 2 to remove a word in dictionary \n " << endl;
        cout << "******-----> enter 3 to highlight a Misspelled word \n"
             << endl;
        cout << "******-----> enter 4 for selecting the appropriate suggestion to replacing the misspelled word with the correct one \n " << endl;
        cout << "******-----> enter 5 to automatically update the paragraph  \n " << endl;
        cout << "******-----> enter 6 to word count  \n " << endl;
        cout << "********** Enter the number corresponding to your need ********** \n"
             << endl;
        cout << "***************************************************************************************************************************************\n"
             << endl;
        cin >> condition; // asking user for the enter a word to perform the opration what they need
        cout << endl;
        switch (condition) // usinf switch case for the perform the given task
        {
            //--------------------------------------------for custom dictionary (add the word in dictionary)--------------------------------------------//

        case 1:
        {
            string add;
            int number;
            if (condition == 1)
            {
                cout << "How many word you want to add in dictionary : " << endl;
                cin >> number;
                for (int i = 0; i < number; i++)
                {
                    cin >> add;
                    spl.addWord(add);
                }
                cout << "Word added successfully in dictionary" << endl;
            }
            break;
        }

            //--------------------------------------------for custom dictionary (remove the word from dictionary)----------------------------------//

        case 2:
        {
            cout << "enter the word that you want to remove " << endl;
            string remove;
            cin >> remove;
            spl.removeWordFromDictionary(remove);
            cout << "Word successfully deleted" << endl;
            break;
        }

            //--------------------------------------------for highlighting the misspelled word---------------------------------------------------//

        case 3:
        {

            ifstream fin(filename);
            ofstream fout("corrected_output.txt");
            string word;
            if (fin.is_open() && fout.is_open())
            {
                while (fin >> word)
                {
                    if (word[(word.length() - 1)] == ',' || word[(word.length() - 1)] == '.' || word[(word.length() - 1) == '!'])
                    {
                        word = removespecialchar(word);
                    }

                    if (spl.searchWord(word))
                    {
                        cout << word << " ";
                    }
                    else
                    {

                        cout << "\033[1;31m " << word << " \033[0m";
                    }
                }
                fin.close();  // close the file
                fout.close(); // close the file
            }
            else
            {
                cout << "Unable to open input or output file." << endl;
                return 1;
            }
            break;
        }


             case 4:

        {
            ifstream in(filename);
            ofstream out("corrected_text.txt");
            if (in.is_open() && out.is_open())
            {
                while (in >> word)
                {
                    wordcount++;
                    if (spl.searchWord(word))
                    {
                        out << word << " ";
                    }
                    else
                    {
                        vector<string> suggestions = spl.getWordSuggestions(word);

                        if (!suggestions.empty())
                        {
                            cout << " \n Suggestions for '" << word << "': \n"
                                 << endl;

                            for (size_t i = 0; i < suggestions.size(); ++i)
                            {
                                cout << i + 1 << ".   " << suggestions[i] << endl;
                            }
                            cout << endl;
                            cout << "Enter the number corresponding to the correct spelling or 0 to keep the original word: ";
                            int choice;
                            cin >> choice;
                            if (choice >= 1 && choice <= suggestions.size())
                            {
                                word = suggestions[choice - 1];
                            }
                        }
                        out << word << " ";
                    }
                }
                in.close();
                out.close();
            }
            else
            {
                cout << "Unable to open input or output file." << endl;
                return 1;
            }
            cout << endl;
            cout << "** Text has been corrected and saved to 'corrected_text.txt'. **" << endl;
            break;
        }

            //--------------------------------------------for auto correct the paragraph--------------------------------//

        case 5:
        {
            cout << " WARNING : you may or may not get corrected spelling of your words \n"
                 << endl;
            ifstream inputFile(filename);
            ofstream outputFile("corrected_output.txt");
            string word;
            if (inputFile.is_open() && outputFile.is_open())
            {
                while (inputFile >> word)
                {
                    wordcount++;
                    vector<string> suggestions = spl.getWordSuggestions(word);
                    if (!suggestions.empty())
                    {
                        // Automatic correction logic
                        if (suggestions.size() == 1)
                        {
                            word = suggestions[0]; // Automatically choose the only suggestion
                        }
                        else
                        {
                            int minDistance = numeric_limits<int>::max();
                            string closestMatch;
                            for (const auto &suggestion : suggestions)
                            {
                                int distance = spl.computeEditDistance(word, suggestion);
                                if (distance < minDistance)
                                {
                                    minDistance = distance;
                                    closestMatch = suggestion;
                                }
                            }
                            word = closestMatch; // Automatically choose the closest match
                        }
                    }
                    outputFile << word << " ";
                }
                inputFile.close();  // close the file
                outputFile.close(); // close the file
            }
            else
            {
                cout << "Unable to open input or output file." << endl;
                return 1;
            }

            cout << "Text has been corrected and saved to 'corrected_output.txt'. Thank you for using our spelling correction program. \n"
                 << endl;
            break;
        }

            //--------------------------------------------for count the word in the output file------------------------------------------------------------//

        case 6:
        {
            ifstream fin(filename);
            if (fin.is_open())
            {
                while (fin >> word)
                {
                    wordcount++;
                }
                fin.close();
            }
            cout << "Word Count : " << wordcount << endl;
            break;
        }

            //--------------------------------------------if case doesn't match------------------------------------------------------------//

        default:
        {
            if (condition == 0)
            {
            }
            else
                cout << "please enter the valid number" << endl;
            break;
        }
        }

    } while (condition); // enter 0 to break the loop

    cout << endl
         << endl;
    cout << "***********************   THANK FOR THE VISITING OUR SPELL CHECKER PROGRAMMM     ************************" << endl;

    return 0;
}
