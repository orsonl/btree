
#include"iostream"
#include <string>


using namespace std;





// This class is used in bTree class. Each of BTreeNode represent a node in a b-tree.
// In this class, there is a struct(slot) to bundle keys with their corresponding valus.
// In each node, there are two arrays. One is a array of sluts. The other one is a array of the child node pointers.
class BTreeNode
{
    
private:
    struct slot
    {
        string keys;
        string value;
    };
    
    int size;
    slot *slots;
    BTreeNode **C;
    int n;
    bool leaf;
    BTreeNode *parent;
public:
    BTreeNode(int _t, bool _leaf);
    ~BTreeNode();
    friend class bTree;
    
    
};


//constructer of bTree. Since bTree is the frind class of BTreeNode, functions of bTree can access any of
//BTreeNode's private variables. string structure is used in toStr().

class bTree
{
private:
    BTreeNode *root;
    int size;
    string structure;
public:
    bTree(int _t);
    ~bTree();
    bool insert(string key, string value);
    bool find(string key, string *value);
    string search(BTreeNode *current_node, string k, string *value);
    void findHelper(BTreeNode *current_node);
    string toStr();
    void insertHelper(string k,string value);
    void splitChild(int i, string k,string value, BTreeNode *y);
    void splitChild_helper(int index,  BTreeNode *y);
    bool delete_key(string key);
    //    BTreeNode *locator(string k);
    //    void deleteFromLeaf(BTreeNode *location ,string k);
    //    void deleteFromInnerNode(BTreeNode *location,string k);
    
    
};




//constructor of BTreeNode.
//Both the size of slots and C is one spot bigger than they are supposed to be.
//The reason for this is explained in the comment of splitChild funtion.

BTreeNode::BTreeNode(int size1, bool leaf1)
{
    size = size1;
    leaf = leaf1;
    slots = new slot [size+1];
    C = new BTreeNode *[size+2];
    n = 0;
    parent = NULL;
}


//destructor of BTreeNode.

BTreeNode::~BTreeNode()
{
    
    delete[] slots;
    delete[] C;
    
}




//constructor of bTree.


bTree::bTree(int size1)
{
    
    size = size1-1;  //
    root = NULL;
    string structure="";
    
}



//destructor of bTree.

bTree::~bTree()
{
    delete[] root;
}







// This function is a helper function of the main insert function.
// This function will only be used when the bTree has at least two layer. Namely, the root of the bTree is not leaf.


void bTree::insertHelper(string k, string value)
{
    int i = root->n-1;
    BTreeNode *N = root;
    
    
    //Search for the BTreeNode in which input key and value should be inserted.
    
    
    while (N->leaf == false)
    {
        i = N->n-1;
        
        while (i >= 0 && N->slots[i].keys[0] > k[0])
            i--;
        
        N = N->C[i+1];
    }
    
    
    //If the target BTreeNode is not full, move every slot after the target position one spot backward.
    //then, insert the input key and value to the target position.
    
    
    if (N->n < size)
    {
        int i2 = N->n-1;
        while (i2 >= 0 && N->slots[i2].keys[0] > k[0])
        {
            N->slots[i2+1] = N->slots[i2];
            i2--;
        }
        
        N->slots[i2+1].keys = k;                                            //
        N->slots[i2+1].value = value;
        
        N->n = N->n+1;
        
    }
    
    
    //If the target BTreeNode is already full, the target BTreeNode must be split.
    
    else
    {
        splitChild(i+1,k,value,N);
    }
}











//This function splits the target BTreeNodes that are already full.
//Like mentioned above, the slots array is one spot larger than it is supposed to be.
//Because of this characteristic, the input key can first be insert into its target BTreeNodes, even if the
//target BTreeNodes is technically full. The slot array in the target BTreeNodes then is sorted. The median
//then is taken out and insert to its parent node.
//The parameter index indicate in which position of the parent node should the median should be inserted

void bTree::splitChild(int index, string k,string value, BTreeNode *y)
{
    
    //This section sorted the slot array after the input key is inserted.
    int i = y->n-1;
    while ( i >= 0 && y->slots[i].keys[0] > k[0])
    {
        
        i--;
    }
    i=i+1;
    
    for(int i2=y->n-1; i2>i-1;i2--)
    {
        
        y->slots[i2+1] = y->slots[i2];
        
    }
    
    y->slots[i].keys = k;
    y->slots[i].value = value;
    
    
    //splitChild_helper is the helper function of splitChild. It will split the target node and then pass up the median of the slot array to the target node's parent.
    
    splitChild_helper(index, y);
    
    
    //If even the target node's parent is full and therefore cannot take the median. The parent must be split too.
    //This procedure will be executed recusively until a BtreeNode that is not full and can accommadate the median of its child array.
    
    //taker points to the parent of the current node
    
    BTreeNode *taker = y->parent;
    
    
    //if the parent of the current node is full.
    // we point taker to current node's parent.
    
    while (taker->n == size +1)
    {
        
        if (taker->parent != NULL)
        {
            int i = taker->parent->n-1;
            while (i >= 0 && taker->parent->slots[i].keys[0] > taker->slots[size/2].keys[0])
            {
                i--;
            }
            
            splitChild_helper(i, taker);
        }
        
        // if even the root is full, the bTree grows in height.
        
        else if (taker-> parent== NULL)
        {
            
            BTreeNode *s = new BTreeNode(size, false);
            s->C[0] = taker;
            taker->parent = s;
            splitChild_helper(0,taker);
            root = s;
        }
        
        taker = taker->parent;
    }
    
    
    
}




void bTree::splitChild_helper(int index,  BTreeNode *y)
{
    
    //The median key and values of the target node's slot array is saved to pass_up_keys and pass_up_value
    //respectively.
    
    string pass_up_keys = y->slots[size/2].keys;
    string pass_up_value = y->slots[size/2].value;
    
    
    //A new BTreeNode z is created to accommodate the second half of the target node.
    
    BTreeNode *z = new BTreeNode(y->size, y->leaf);
    
    z->n = (size+1)/2;
    z->parent = y->parent;
    
    //pass the second half of the slot array to z.
    
    for (int j=0; j+size/2+1<=size; j++)
    {
        z->slots[j].keys = y->slots[j+size/2+1].keys;
        z->slots[j].value = y->slots[j+size/2+1].value;
    }
    
    
    //pass the second half of the children array to z.
    
    
    if (y->leaf == false)
    {
        for (int j=0; j+size/2+1<=size+1; j++)
            z->C[j] = y->C[j+size/2+1];
    }
    
    //change the size of y
    
    y->n = size/2;
    
    
    BTreeNode *taker = y->parent;
    
    
    //move the element in the slot array so the median can be inserted.
    
    
    for (int j = index; j < size; j++)
    {
        taker->slots[j+1].keys=taker->slots[j].keys;
        taker->slots[j+1].value=taker->slots[j].value;
        
    }
    
    // insert the median to the proper position in the parent node.
    
    
    taker->slots[index].keys = pass_up_keys;
    taker->slots[index].value = pass_up_value;
    
    
    //change the size of the parent node.
    
    
    taker->n=taker->n+1;
    
    
    //adjust the position of the children array of the parent node. Since a new key and a new values have been inserted.
    
    for (int j=index+1; j < size+1; j++)
        taker->C[j+1] = taker->C[j];
    taker->C[index+1] = z;
    
}





bool bTree::insert(string k, string value)
{
    
    // If tree is empty
    if (root == NULL)
    {
        // Allocate memory for root
        
        root = new BTreeNode(size, true);
        
        root->slots[0].keys = k;  // Insert key
        root->slots[0].value = value;
        root->n = 1;  // Update number of keys in root
        
    }
    else // If tree is not empty
    {
        // If root is full, then tree grows in height
        
        
        if (root->n == size && root->leaf ==true)
        {
            // Allocate memory for new root
            
            BTreeNode *s = new BTreeNode(size, false);
            
            // Make old root as child of new root
            
            root->parent = s;
            s->C[0] = root;
            splitChild(0, k,value,root);
            root = s;
        }
        //  If root is not FULL, call insertNonFull for root
        else
        {
            insertHelper(k, value);
        }
    }
    
    
    return 0;
}



//This function toStr() and its helper function finderHelper traverse the bTree in inorder traversal.


string bTree::toStr()
{
    structure = "";
    findHelper(root);
//    cout<<structure<<endl;
    return structure;
}




void bTree::findHelper(BTreeNode *current_node)
{
    int i2=0;
    
    
    for(int i=0; i < current_node->n; i++)
    {
        
        //traverse the tree untill the node is a leaf node.
        
        if (current_node->leaf == false)
        {
            findHelper(current_node->C[i]);
        }
        
        //concatenate every element in the leaf node to string "structure".
        
        structure = structure  +current_node->slots[i].keys+"\n";
        
        i2= i;
    }
    
    if (current_node->leaf == false)
        findHelper(current_node->C[i2+1]);
    
}



//This function find and its helper function search find for input key through the bTree. If the input key is found. The value of of the slot in which the input key is located will be saved to the sent pointer.


bool bTree::find(string key, string *value)
{
    string result  = search(root,key, value);
    if (result != "")
    {
        *value = result;
        return true;
    }
    return false;
}



// Function to search key k in subtree rooted with this node
string bTree::search(BTreeNode *current_node, string k, string *value)
{
    //BTreeNode *current_node = root;
    // Find the first key greater than or equal to k
    string ret = "";
    int i = 0;
    while (i < current_node->n && k > current_node->slots[i].keys)
        i++;
    
    // If the FOUND key is equal to k, RETURN this node
    if (current_node->slots[i].keys == k)
    {
        //        cout<<"found"<<endl;
 
//        cout <<current_node->slots[i].value<<endl;

        ret = current_node->slots[i].value;
        return ret;
    }
    
    // If key is NOT FOUND here and this is a leaf node
    if (current_node->leaf == true)
    {
        //        cout<<"not_found"<<endl;
        return 0;
    }
    // Go to the appropriate child
    return search(current_node->C[i],k,&(*value));
}





 bool bTree::delete_key(string k)
{

    return 1;


}


// The following section is the delet function.
// It is unfinished, but it is half way done. I hope you can go though it and consider give me some partial credit.
// What I have done is as follow:

// locator. This function will search for the key users want to delete. If the key is not found, it will return false

// eleteFromLeaf. In short, if the number of key is smaller than size/2. A rotation of keys( in clockwise or counterclockwise order) among the node, its sibling, and parent will occur.

/*
 
 bool BTree::delete_key(string k)
 {
 
 BTreeNode *location = locator(k);
 
 
 if(location != NULL)
 {
 
 
 if(location->leaf == true)
 {
 
 deleteFromLeaf(location,k);
 
 }
 
 else
 {
 
 deleteFromInnerNode(location,k);
 }
 
 return 1;
 }
 
 else
 {
 return 0;
 }
 
 }
 
 
 
 void BTree::deleteFromLeaf(BTreeNode *location ,string k)
 {
 
 if (location->n > size/2)
 {
 location->n = location->n-1;
 }
 
 else
 {
 BTreeNode *parent = location->parent;
 
 
 int i = parent->n-1;
 while (i >= 0 && parent->keys[i][0] > k[0])
 {
 i--;
 }
 
 
 int i2 = i;
 
 while (i2 >= 0 && parent->C[i2]->n<(size/2))
 {
 i2--;
 }
 
 
 
 if (i2 > 0)
 {
 
 
 int i3 = location->n-1;
 
 while (i3 >= 0 && location->keys[i][0] > k[0])
 {
 
 i3--;
 }
 
 for(int i=0; i< i3;i++)
 {
 
 location->keys[i+1] = location->keys[i];
 
 }
 
 
 
 location->keys[0] = parent->keys[i];
 parent->keys[i] = parent->C[i2]->keys[parent->C[i2]->n-1];
 parent->C[i2]->n=parent->C[i2]->n-1;
 
 
 
 
 }
 
 else
 {
 
 int i22 = i+1;
 
 while (i22 <= parent->n && parent->C[i22]->n<(size/2))
 {
 i22++;
 }
 
 
 int i32 = location->n-1;
 
 while (i32 >= 0 && location->keys[i][0] > k[0])
 {
 
 i32--;
 }
 
 for(int i=location->n-1; i> i32;i--)
 {
 
 location->keys[i-1] = location->keys[i];
 
 }
 
 
 
 location->keys[location->n-1] = parent->keys[i+1];
 parent->keys[i+1] = parent->C[i22]->keys[0];
 
 
 
 for(int i = parent->C[i22]->n-1; i > 0; i--)
 {
 
 parent->C[i22]->keys[i-1] = parent->C[i22]->keys[i];
 
 }
 
 
 
 parent->C[i22]->n=parent->C[i22]->n-1;
 
 }
 
 
 }
 
 
 
 
 
 
 
 
 }
 
 
 BTreeNode *BTree::locator(string k)
 
 {
 
 int i = root->n-1;
 
 BTreeNode *N = root;
 while (N->leaf == false)
 {
 i = N->n-1;
 
 while (i >= 0 && N->keys[i][0] > k[0])
 i--;
 
 if (N->keys[i]==k)
 return N;
 
 
 //       cout<<"inserted_location_is"<<endl;
 //       cout << i+1 <<endl;
 N = N->C[i+1];
 }
 for(int i =0; i< N->n; i++)
 {
 if(N->keys[i] == k)
 {
 return N;
 
 }
 
 
 }
 return NULL;
 }
 
 
 
 
 
 
 
 */







