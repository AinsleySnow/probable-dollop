#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

int findMatchedRight(const string &line, int pos);
int findMatchedLeft(const string &line, int pos);

int main(int argc, char *args[])
{
    if (argc != 1)
    {
    }
    while (true)
    {
        cout << "Input a brainf**k line." << endl
             << "Input \"exit\" to exit." << endl;
        
        string perLine;
        getline(cin, perLine);
        if (perLine == "exit")
            return 0;
        
        size_t size = 10;
        char *buffer = new char[size]{};
        
        int pos = 0;
        for (int i = 0; i < perLine.length();)
        {
            switch (perLine[i])
            {
            case '+':
                buffer[pos]++;
                break;
            case '-':
                buffer[pos]--;
                break;
            case '>':
                pos++;
                if (pos > size) //if allocated memory is used up
                {
                    size += 10;
                    buffer = static_cast<char *>(realloc(buffer, size)); //just extend the memory
                }
                break;
            case '<':
                pos--;
                if (pos < 0) //obviously a minus 'pos' is invalid
                {
                    cout << endl
                         << "Oops! Over flow error!";
                    return 1;
                }
                break;
            case '.':
                cout << buffer[pos];
                break;
            case ',':
                buffer[pos] = getchar();
                break;
            case '[':
                if (buffer[pos] == 0)
                {
                    i = findMatchedRight(perLine, i + 1) + 1;
                    if (i == -1)
                    {
                        cout << endl << "Unexpected symbol '['.";
                        goto end;
                    }
                    continue;
                }
                break;
            case ']':
                i = findMatchedLeft(perLine, i - 1);
                if (i == -1)
                {
                    cout << endl << "Unexpected symbol ']'.";
                    goto end;
                }
                continue;
            default:
                cout << "Undefined symbol"
                     << "'" << perLine[i] << "'.";
                return 1;
            }
            ++i;
        }
    end:
        delete[] buffer;
        buffer = nullptr;
    }
    return 0;
}

//find matched ']'; returns -1 if nothing found
int findMatchedRight(const string &line, int pos)
{
    int left = 0;
    for (; pos < line.length(); ++pos)
    {
        if (line[pos] == '[')       //A '[' means that there is a nested '[]' blocks...
            ++left;                 //So we increase the value of 'left' to mark it.
        else if (line[pos] == ']')  //if we found a ']'
        {
            if (!left)              //left == 0 means the ']' we found does match initial '['
                return pos;
            else                    //In this case, the ']' we found marks the end of a nested block
                --left;             //So we decrease 'left',  means we have reached the end of one nested block
        }
    }
    return -1;
}

//find matched '['; returns -1 if nothing found 
int findMatchedLeft(const string &line, int pos)
{
    int right = 0;
    for (; pos >= 0; --pos)
    {
        if (line[pos] == ']')
            ++right;
        else if (line[pos] == '[')
        {
            if (!right)
                return pos;
            else
                --right;
        }
    }
    return -1;
}