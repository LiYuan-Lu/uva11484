// uva_11484.cpp : Defines the entry point for the console application.
//

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

std::vector<std::string> split(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

struct Object
{
    Object(std::string name)
        : name_(name)
        , parent_(nullptr)
        , next_(nullptr)
        , priv_(nullptr)
        , first_child_(nullptr)
        , last_child_(nullptr)

    {

    }
    std::string name_;
    Object * parent_;
    Object * next_;
    Object * priv_;
    Object * first_child_;
    Object * last_child_;
};

class DocumentObjectModel
{
public:
    DocumentObjectModel();
    ~DocumentObjectModel();

public:
    void parse(std::istream & input, std::ostream & output);

private:
    void parse_element(std::string line);
    void parse_command(std::string & line, std::ostream & os);
    void clear();

private:
    Object * current_element_;
    Object * head_;
    Object * head_for_clear_;
    int case_count_;
};

DocumentObjectModel::DocumentObjectModel()
    : current_element_(nullptr)
    , head_(nullptr)
    , head_for_clear_(nullptr)
    , case_count_(1)
{
}

DocumentObjectModel::~DocumentObjectModel()
{
    clear();
}

void DocumentObjectModel::parse_element(std::string line)
{
    if (line != "</n>")
    {
        auto splits = split(line, '\'');
        if (splits.size() == 3)
        {
            std::string name = splits[1];
            if (!current_element_)
            {
                current_element_ = new Object(name);
                if (!head_)
                {
                    head_ = current_element_;
                    head_for_clear_ = current_element_;
                }
            }
            else
            {
                Object * child = new Object(name);
                child->parent_ = current_element_;
                if (!current_element_->first_child_)
                {
                    current_element_->first_child_ = child;
                }
                if (current_element_->last_child_)
                {
                    current_element_->last_child_->next_ = child;
                    child->priv_ = current_element_->last_child_;
                }
                current_element_->last_child_ = child;
                current_element_ = child;
            }
        }
    }
    else
    {
        current_element_ = current_element_->parent_;
    }
}

void DocumentObjectModel::parse_command(std::string & line, std::ostream & os)
{
    if (line == "first_child")
    {
        if (head_->first_child_)
        {
            head_ = head_->first_child_;
        }
        os << head_->name_ << std::endl;
    }
    else if (line == "next_sibling")
    {
        if (head_->next_)
        {
            head_ = head_->next_;
        }
        os << head_->name_ << std::endl;
    }
    else if (line == "previous_sibling")
    {
        if (head_->priv_)
        {
            head_ = head_->priv_;
        }
        os << head_->name_ << std::endl;
    }
    else if (line == "parent")
    {
        if (head_->parent_)
        {
            head_ = head_->parent_;
        }
        os << head_->name_ << std::endl;
    }
}

void DocumentObjectModel::clear()
{
    Object * current = head_for_clear_;
    while (current)
    {
        if (current->first_child_)
        {
            current = current->first_child_;
        }
        else if (current->next_)
        {
            Object* tmp = current;
            current = current->next_;
            delete tmp;
        }
        else
        {
            Object* tmp = current;
            current = current->parent_;
            if (current)
            {
                current->first_child_ = nullptr;
            }
            delete tmp;
        }
    }

    head_for_clear_ = nullptr;
    head_ = nullptr;
    current_element_ = nullptr;
}

void DocumentObjectModel::parse(std::istream & input, std::ostream & output)
{
    std::string line;
    std::getline(input, line);
    int number_of_lines = stoi(line);
    while (number_of_lines != 0)
    {
        for (int i = 0; i < number_of_lines; i++)
        {
            std::getline(input, line);
            parse_element(line);
        }

        std::getline(input, line);
        int number_of_command = stoi(line);

        output << "Case " << (case_count_++) << ":" << std::endl;

        for (int i = 0; i < number_of_command; i++)
        {
            std::getline(input, line);
            parse_command(line, output);
        }

        clear();

        do 
        {
            std::getline(input, line);
        } while (line.empty());
            
        number_of_lines = stoi(line);
    }
}

int main()
{
    DocumentObjectModel dom;
#ifdef ONLINE_JUDGE
    dom.parse(std::cin, std::cout);
#else
    std::ifstream input("input.txt");
    std::ofstream output("output.txt");
    dom.parse(input, output);
#endif
    return 0;
}
