/**********************************************************************
 * @file ReminderTree.h
 * @author TomatoPi
 * @version 1.0
 *
 * Tree Data Structure addapted to store basic datas in hierarchical
 * order
 * Data stored must be self descriptive, because tree structure only
 * provide a memory function
 * Basicaly used to see if a specific sequence of Value already exist
 **********************************************************************/
 #include "ReminderTree.h"

/**
 * Tree Constructor, take value or object that will be returned
 * as error and valid Value for all tree's functions
 **/
template <typename Val, uint8_t Depth>
ReminderTree<Val,Depth>::ReminderTree( Val okVal, Val errVal ):
    m_root(new ReminderTree<Val,Depth>::Node<Val>()),
    m_okValue(okVal),
    m_errValue(errVal)
{
    //printf("New Reminder Tree Created\n");
}

template <typename Val, uint8_t Depth>
ReminderTree<Val,Depth>::~ReminderTree(){

    this->clear();
    delete m_root;
}

template <typename Val, uint8_t Depth>
void ReminderTree<Val,Depth>::clear(){

    eraseNode(m_root);
    m_root = new Node<Val>();
}
/**
 * Function that return Value at given path, or return
 * valid Value if given sequence exist
 * error Value if not
 **/
template <typename Val, uint8_t Depth>
Val ReminderTree<Val,Depth>::find( Val seq[Depth] ){

    //printf("Search for Given sequence inside the Tree\n");
    Node<Val> *cur = m_root;

    for ( uint8_t i = 0; i < Depth; i++ ){

        //printf("  Depth ( %d ) Search for Node ( %d ) -- ", i, seq[i] );
        if ( cur->m_node.find(seq[i]) == cur->m_node.end() ){

            //printf("Not Founded\n");
            return m_errValue;
        }
        //printf("Founded\n");
        cur = cur->m_node[seq[i]];
    }
    return m_okValue;
}

/**
 * Add given sequence to the tree
 * Return error value if given sequence already exist
 **/
template <typename Val, uint8_t Depth>
Val ReminderTree<Val,Depth>::add( Val seq[Depth] ){

    //printf("Add Given Sequence to the Tree :\n");
    Node<Val> *cur = m_root;
    
    for ( uint8_t i = 0; i < Depth; i++ ){

        if ( cur->m_node.find(seq[i]) == cur->m_node.end() ){

            //printf("  D( %d ) N( %d ) -- Added Node\n", i, seq[i] );
            cur->m_node[seq[i]] = new Node<Val>();
        }
        else{
            //printf("  D( %d ) N( %d ) -- Node Exist\n", i, seq[i] );
        }
        cur = cur->m_node[seq[i]];
    }
    return m_okValue;
}

/**
 * Remove given sequence from the tree
 * Return error value if given sequence don't exist
 **/
template <typename Val, uint8_t Depth>
Val ReminderTree<Val,Depth>::remove( Val seq[Depth], uint8_t depth ){

    Node<Val> *cur = m_root;

    // Search for node to remove
    for ( uint8_t i = 0; i < Depth; i++ ){

        //printf("  Depth ( %d ) Search for Node ( %d ) -- ", i, seq[i] );
        if ( cur->m_node.find(seq[i]) == cur->m_node.end() ){

            //printf("Not Founded\n");
            return m_errValue;
        }
        //printf("Founded\n");
        
        // When founded erase it
        if ( i == depth ){
            
            //Keep parent node pointer
            Node<Val> *bak = cur;
            cur = cur->m_node[seq[i]];

            // Remove key from parent
            bak->m_node.erase(seq[i]);
            // Remove Child
            eraseNode(cur);
            // Stop searching
            break;
        }
        cur = cur->m_node[seq[i]];
    }
    
    return m_okValue;
}

/**
 * Function used to get back list of all stored sequencies
 **/
template <typename Val, uint8_t Depth>
std::vector<Val*> ReminderTree<Val,Depth>::getBackTree() const{

    Val d[Depth];
    return getNode( m_root, std::vector<Val*>(), d, 0 );
}

/**
 * Show current Tree Content
 **/
template <typename Val, uint8_t Depth>
void ReminderTree<Val,Depth>::print() const{

    printf("Print Tree Content :\n");
    printNode(m_root, 1); 
    printf("Tree End\n");
}

/**
 * Recursive function that print Tree from given node
 * Depth indication represent current depth inside tree and is
 * only used for output indentation
 **/
template <typename Val, uint8_t Depth>
void ReminderTree<Val,Depth>::printNode( Node<Val> *node, uint8_t d ) const{

    for( typename std::map<Val,Node<Val>*>::iterator cur = node->m_node.begin()
            ; cur != node->m_node.end()
            ; cur++ )
    {

        printf("%*s-Node : ( %d )\n", d, " ", static_cast<int>( cur->first ));
        printNode( cur->second, d+1 );
    }
}

/**
 * Recursive function used to get back list of stored sequencies
 * @param out output array containing recovered sequencies
 * @param prefix prefix containing current position inside tree
 * @param depth current depth inside tree, number of significant
 *  datas inside prefix array
 *
 * @return passed vector pushed front newly recovered sequencies
 **/
template <typename Val, uint8_t Depth>
std::vector<Val*> ReminderTree<Val,Depth>::getNode
    (
    Node<Val> *node,
    std::vector<Val*> out,
    Val prefix[Depth],
    uint8_t depth
    ) const
{
    // For each Entries inside node add current position to prefix
    for( typename std::map<Val,Node<Val>*>::iterator cur = node->m_node.begin()
            ; cur != node->m_node.end()
            ; cur++ )
    {
        prefix[depth] = cur->first;
        // If current depth is maximum depth of tree, add path to out vector
        if ( depth == Depth-1 ){
            Val *sequence = new Val[Depth];
            memcpy( sequence, prefix, sizeof(Val) * Depth );
            out.push_back(sequence);
        }
        // Else add current node to the prefix and enter sub branche
        else{

            out = getNode( cur->second, out, prefix, depth+1 );
        }
    }
    return out;
}

template <typename Val, uint8_t Depth>
void ReminderTree<Val,Depth>::eraseNode( Node<Val> *node ){

    //printf("Erasing node ... ");
    for( typename std::map<Val,Node<Val>*>::iterator cur = node->m_node.begin()
            ; cur != node->m_node.end()
            ; cur++ )
    {
        // If node contains sub nodes erase it
        if ( cur->second->m_node.size() != 0 ){
            //printf("Node contains subNodes :\n");
            eraseNode(cur->second);
        }
        delete cur->second;
    }
    
    delete node;
    //printf("Done\n");
}
