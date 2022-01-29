#include <iostream>
#include <cstring>
using namespace std;
struct PAIR
{
    string word="";
    char part_of_speech;
    float frequency;
    bool operator<(const PAIR value)
    {
        return word < value.word;
    }
    friend ostream &operator<<(ostream &os, PAIR& pair)
    {
        os << pair.word + " " << pair.part_of_speech << " " << pair.frequency;
        return os;
    }
    friend istream &operator>>(istream &is, PAIR& pair)
    {
        is >> pair.word >> pair.part_of_speech >> pair.frequency;
        return is;
    }
};
class RBtree
{
public:
    RBtree()
    {
        PAIR pair;
        NIL=new RBtreeNode(pair);
        NIL->color=BLACK;
        root=NIL;
        freopen("project1\\init.txt", "r", stdin);
        string str;
        getline(cin,str);
        PAIR pair_1;
        while (cin >> pair_1)
        {
            insert(pair_1);
        }
        fclose(stdin);
    };
    class RBtreeNode
    {
    public:
        RBtreeNode *left, *right, *parent;
        PAIR value;
        int color; //0表示红色，1表示黑色
        RBtreeNode(PAIR value, RBtreeNode *left = NULL, RBtreeNode *right = NULL, RBtreeNode *parent = NULL)
        {
            this->value = value;
            this->left = left;
            this->right = right;
            this->parent = parent;
            this->color = RED;
        }
    };
    
    
    RBtreeNode *search(string key)
    {
        RBtreeNode* fp=search_key(key);
        if (fp==NIL) {
            cout << "The key "+key+" is missing" << endl;
            return NULL;
        }
        else
        {
            cout << fp->value << endl;
            return fp;
        }
    }
    void delete_by_command(string key){
        if (search_key(key)==NIL){
            cout << "key "+key+" missing" << endl;
            return;
        }
        else
        {
            RBtreeNode *z=search_key(key),*y,*x;
            if (z->left==NIL || z->right==NIL) y=z;
            else y=successor(z);
            if (y->left!=NIL) x=y->left;
            else x=y->right;
            x->parent=y->parent;
            if (y->parent==NIL)
                root=x;
                else if (y==y->parent->left)
                {
                    y->parent->left=x;
                }
                else y->parent->right=x;
            if (y!=z) z->value=y->value;
            if (y->color==BLACK) delete_fixup(x);
        }
    }
    void delete_by_file(string filepath="project1\\delete.txt"){
        string str;
        PAIR pair;
        freopen(filepath.c_str(),"r",stdin);
        getline(cin,str);
        while (cin>>pair)
        {
            delete_by_command(pair.word);
        }
        fclose(stdin);
    }
    void insert(PAIR pair)
    {
        if (search_key(pair.word) != NIL)
        {
            cout << "key " + pair.word + " conflict" << endl;
            return;
        }
        else
        {
            RBtreeNode *z=new RBtreeNode(pair,NIL,NIL);
            RBtreeNode *y=NIL,*x=root;
            while (x!=NIL)
            {
                y=x;
                if (z->value<x->value) x=x->left;
                    else x=x->right;
            }
            z->parent=y;
            if (y==NIL)
                root=z;
                else if (z->value<y->value)
                {
                    y->left=z;
                }
                else y->right=z;
            insert_fixup(z);
        }
    }
    void insert_byfile(string filepath="project1\\insert.txt"){
        string str;
        PAIR pair;
        freopen(filepath.c_str(),"r",stdin);
        getline(cin,str);
        while (cin>>pair)
        {
            insert(pair);
        }
        fclose(stdin);
    }
    void Dump(){
        Dump(root);
    }
    void update(PAIR pair)
    {
        RBtreeNode* temp=search_key(pair.word);
        if (temp == NIL)
        {
            insert(pair);
        }
        else
        {
            temp->value.part_of_speech = pair.part_of_speech;
            temp->value.frequency = pair.frequency;
        }
    }

private:
    static const int BLACK = 1;
    static const int RED = 0;
    RBtreeNode *root;
    RBtreeNode *NIL;
    void insert_fixup(RBtreeNode* z){
        RBtreeNode *y;
        while (z->parent->color==RED)
        {
            if (z->parent==z->parent->parent->left){     //case1
                y=z->parent->parent->right;
                if (y->color==RED){
                    z->parent->color=BLACK;
                    y->color=BLACK;
                    z->parent->parent->color=RED;
                    z=z->parent->parent;
                    continue;
                }
                else if (z==z->parent->right)       //case2
                {
                    z=z->parent;
                    left_rotation(z);
                }
                z->parent->color=BLACK;         //case3
                z->parent->parent->color=RED;
                right_rotation(z->parent->parent);
            }
            else
            {
                y=z->parent->parent->left;
                if (y->color==RED){
                    z->parent->color=BLACK;
                    y->color=BLACK;
                    z->parent->parent->color=RED;
                    z=z->parent->parent;
                    continue;
                }
                else if (z==z->parent->left)
                {
                    z=z->parent;
                    right_rotation(z);
                }
                z->parent->color=BLACK;
                z->parent->parent->color=RED;
                left_rotation(z->parent->parent);
            }
        }
        root->color=BLACK;
    }
    void left_rotation(RBtreeNode* x){
        RBtreeNode *y=x->right;
        x->right=y->left;
        y->left->parent=x;
        y->parent=x->parent;
        if (x->parent==NIL)
            root=y;
            else if (x==x->parent->left)
            {
                x->parent->left=y;
            }
            else x->parent->right=y;
        y->left=x;
        x->parent=y;
    }
    void right_rotation(RBtreeNode* x){
        RBtreeNode *y=x->left;
        x->left=y->right;
        y->right->parent=x;
        y->parent=x->parent;
        if (x->parent==NIL)
            root=y;
            else if (x==x->parent->left)
            {
                x->parent->left=y;
            }
            else x->parent->right=y;
        y->right=x;
        x->parent=y;
    }
    RBtreeNode* Tree_min(RBtreeNode* x){
        while (x->left!=NIL)
        {
            x=x->left;
        }
        return x;
    }
    RBtreeNode* successor(RBtreeNode* x){
        if (x->right) return Tree_min(x->right);
        RBtreeNode *y=x->parent;
        while (y!=NIL && x==y->right)
        {
            x=y;
            y=x->parent;
        }
        return y;
    }
    RBtreeNode *search_key(const string key){
        RBtreeNode* r=root;
        while (r!=NIL && key!=r->value.word)
        {
            if (key<r->value.word)
                r=r->left;
            else
                r=r->right;
        }
        return r;
    }
    void Dump(RBtreeNode *root)
    {
        if (root == NIL)
            return;
        else
        {
            Dump(root->left);
            cout << root->value << endl;
            Dump(root->right);
        }
    }
    void delete_fixup(RBtreeNode* x){
        RBtreeNode *w,*temp;
        while (x!=root && x->color==BLACK)
        {
            if (x==x->parent->left) {
                w=x->parent->right;
                if (w->color==RED){       //case 1
                    w->color=BLACK;
                    x->parent->color=RED;
                    temp=x->parent;
                    // left_rotation(x->parent);
                    left_rotation(temp);
                    // w=x->parent->right;
                    w=temp->right;
                }
                if (w->left->color==BLACK && w->right->color==BLACK){   //case2
                    w->color==RED;
                    x=x->parent;
                    continue;
                }
                else if (w->right->color==BLACK)   //case3
                {
                    w->left->color=BLACK;
                    w->color=RED;
                    temp=x->parent;
                    right_rotation(w);
                    w=temp->right;
                }
                w->color=temp->color;      //case4
                temp->color=BLACK;
                w->right->color=BLACK;
                left_rotation(temp);
                x=root;
            }
            else
            {
                w=x->parent->left;
                if (w->color==RED){
                    w->color=BLACK;
                    x->parent->color=RED;
                    temp=x->parent;
                    // right_rotation(x->parent);
                    right_rotation(temp);
                    // w=x->parent->left;
                    w=temp->left;
                }
                if (w->left->color==BLACK && w->right->color==BLACK){
                    w->color==RED;
                    x=x->parent;
                    continue;
                }
                else if (w->left->color==BLACK)
                {
                    w->right->color=BLACK;
                    w->color=RED;
                    temp=x->parent;
                    left_rotation(w);
                    w=temp->left;
                }
                w->color=temp->color;
                temp->color=BLACK;
                w->left->color=BLACK;
                right_rotation(temp);
                x=root;
            }
        }
        x->color=BLACK;
    }
};
int main(int argc, char const *argv[])
{
    RBtree Tree;
    Tree.insert_byfile();
    Tree.delete_by_file();
    // Tree.search("dead-eyed");
    // Tree.search("a-ha");
    Tree.delete_by_command("report");
    Tree.delete_by_command("report");
    Tree.search("report");
    // Tree.delete_by_command("overprice");
    // Tree.Dump();
    return 0;
}
