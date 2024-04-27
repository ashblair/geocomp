#pragma once
#include <stdexcept>
#include <exception>

namespace AVLX
{
    class avlexception: public std::exception
    { 
    public:
        enum avlExcEmumType {
            IDX_OUT = 0, 
            ROOT = 1, 
            PATH_DIR = 2, 
            RIGHT_NODE = 3,
            LEFT_NODE = 4,
            BALANCE = 5,
            PATH_EMPTY = 6,
            PATH_NODE = 7,
            AVL_INS = 8,
            DATA_NODE = 9,
            DEL_DATA_NODE = 10,
            NODE_DEL = 11,
            DATA_DEL = 12,
            ROOT_DEL = 13,
            AVL_ROOT = 14
            };
        const char * a_Exception[15] = {
            "Error in vector_c - index out of range",
            "Error in AVL::Insert - root not found",
            "Error in AVL - path direction invalid",
            "Error in AVL::Insert - right node already set",
            "Error in AVL::Insert - left node already set",
            "Error in AVL - balance factor invalid",
            "Error in AVL::Delete - path is empty",
            "Error in AVL::Delete - invalid path node",
            "Error in avlNexus::Defrag - defrag avl insert failed",
            "Error in avlNexus::Defrag - data/node imbalance",
            "Error in avlNexus::Defrag - deleted data/node imbalance",
            "Error in avlNexus::Defrag - invalid node delete",
            "Error in avlNexus::Defrag - invalid data delete",
            "Error in avlNexus::Defrag - invalid root delete",
            "Error in avlNexus::getNext - avl/root imbalance"
        };
    ;
        //char * pExc;
    };

    class idx_out: public avlexception
    {
    public:
        virtual const char* what() const throw() {return a_Exception[IDX_OUT];};
    };

    class root: public avlexception
    {
    public:
        virtual const char * what() const throw() {return a_Exception[ROOT];};
    };

    class path_dir: public avlexception
    {
    public:
        virtual const char * what() const throw() {return a_Exception[PATH_DIR];};
    };

    class right_node: public avlexception
    {
    public:
        virtual const char * what() const throw() {return a_Exception[RIGHT_NODE];};
    };

    class left_node: public avlexception
    {
    public:
        virtual const char * what() const throw() {return a_Exception[LEFT_NODE];};
    };

    class balance: public avlexception
    {
    public:
        virtual const char * what() const throw() {return a_Exception[BALANCE];};
    };

    class path_empty: public avlexception
    {
    public:
        virtual const char * what() const throw() {return a_Exception[PATH_EMPTY];};
    };

    class path_node: public avlexception
    {
    public:
        virtual const char * what() const throw() {return a_Exception[PATH_NODE];};
    };

    class avl_insert: public avlexception
    {
    public:
        virtual const char * what() const throw() {return a_Exception[AVL_INS];};
    };

    class data_node: public avlexception
    {
    public:
        virtual const char * what() const throw() {return a_Exception[DATA_NODE];};
    };

    class del_data_node: public avlexception
    {
    public:
        virtual const char * what() const throw() {return a_Exception[DEL_DATA_NODE];};
    };

    class node_del: public avlexception
    {
    public:
        virtual const char * what() const throw() {return a_Exception[NODE_DEL];};
    };

    class data_del: public avlexception
    {
    public:
        virtual const char * what() const throw() {return a_Exception[DATA_DEL];};
    };

    class root_del: public avlexception
    {
    public:
        virtual const char * what() const throw() {return a_Exception[ROOT_DEL];};
    };

    class avl_root: public avlexception
    {
    public:
        virtual const char * what() const throw() {return a_Exception[AVL_ROOT];};
    };
};
