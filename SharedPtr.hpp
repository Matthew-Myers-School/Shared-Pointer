#include <iostream>#include <time.h>#include <string>#include <sstream>#include <climits>#ifndef SHAREDPTR_HPP#define SHAREDPTR_HPPnamespace cs540{            //==============================================================================class Reference{public:    std::atomic<int> ref;    Reference(){        ref = 0;    }    virtual ~Reference() {}};template <typename T>class ReferenceObject : public Reference {public:    T* tData;            ReferenceObject()    {        tData = nullptr;        ref = 0;    }    ReferenceObject(T* temp)    {        tData = temp;        ref = 1;    }    ~ReferenceObject()    {        delete tData;    }};    template<typename T>class SharedPtr{    private:    public:    Reference *refObj;    T * tData;    SharedPtr()    {        //printf("%s\n", __PRETTY_FUNCTION__);        tData = nullptr;        refObj = nullptr;    };                ///Smart Pointer to OBJ, REF count to 1    template <typename U> explicit SharedPtr(U *p)    {        //NEED TO CHECK IF THE POINTER BEING PASSED IN IS NULL?        //printf("%s\n", __PRETTY_FUNCTION__);        tData = (T*)(p);        refObj = new ReferenceObject<U>(p);    }                //TWO CONSTRUCTORS BELOW:    //  IF P != NULL ref count of the obj is incremented    //  This pointer set equal to the pointer being passed in    SharedPtr(const SharedPtr &p)    {        //printf("%s\n", __PRETTY_FUNCTION__);        if (p.refObj != NULL)            ++(p.refObj->ref);        tData = (p.tData);        refObj = p.refObj;    };    template <typename U> SharedPtr(const SharedPtr<U> &p)    {        //printf("%s\n", __PRETTY_FUNCTION__);        if (p.refObj != NULL)            ++(p.refObj->ref);        tData = (T*)(p.tData);        refObj = (ReferenceObject<T>*)p.refObj;    };        //TWO CONSTRUCTORS BELOW:    //  IF P != NULL move the obj over, old pointer must be set to NULL    //  Ref count DOES NOT change    SharedPtr(SharedPtr &&p)    {        //printf("%s\n", __PRETTY_FUNCTION__);        tData = (p.tData);        refObj = p.refObj;        p.tData = nullptr;        p.refObj = nullptr;    };    template <typename U> SharedPtr(SharedPtr<U> &&p)    {        //printf("%s\n", __PRETTY_FUNCTION__);        tData = (T*)(p.tData);        refObj = (ReferenceObject<T>*)p.refObj;        p.tData = nullptr;        p.refObj = nullptr;    };                    //NEXT TWO FUNCS: COPY ASSIGNMENTS    //   Must handle self assignment?    //   DECREMENT REF COUNT OF THE CURRENT SHARED POINTER OBJ IF NOT NULL    //   INCREMENT REF COUNT OF THE NEW POINTER    //   U* NOT IMPLICITLY CONVERTIBLE TO T*, syntax error?        SharedPtr &operator=(const SharedPtr &p)    {        //printf("%s\n", __PRETTY_FUNCTION__);                if (this == &p)        {            return *this;        }        if(refObj != NULL)        {                if(refObj->ref == 1)                {                    refObj->ref = 0;                    delete refObj;                    refObj = nullptr;                    tData = nullptr;                }                else if ( refObj->ref > 1)                {                    --(refObj->ref);                }        }        tData = p.tData;        refObj = p.refObj;        if (refObj != NULL)        {            ++(refObj->ref);        }                return *this;    };   template <typename U> SharedPtr<T> &operator=(const SharedPtr<U> &p)    {        //printf("%s\n", __PRETTY_FUNCTION__);        if (this == (SharedPtr<T>*)&p)        {            return *this;        }        if(refObj != NULL)        {                if(refObj->ref == 1)                {                    refObj->ref = 0;;                    delete refObj;                    refObj = nullptr;                    tData = nullptr;                }                else if ( refObj->ref > 1)                {                    --(refObj->ref);                }        }        //Def need this cast        tData = (T*)p.tData;        refObj = (ReferenceObject<T>*)p.refObj;        // THIS IS IS THE CAUSE OF LEAKS        if (refObj != NULL)        {            ++(refObj->ref);        }                return *this;    };        //NEXT TWO FUNCS: MOVE ASSIGNMENT    //     P MUST BE NULL AFTER    //     REF COUNT STAYS THE SAME    //     IF P IS NULL, SET TO NULL ( DO I NEED TO SET TO NULL OR = TO OTHER REF OBJ? )??        SharedPtr &operator=(SharedPtr &&p)    {        //printf("%s\n", __PRETTY_FUNCTION__);        if (refObj != NULL)        {            if (refObj->ref == 1)            {                refObj->ref = 0;                delete refObj;                refObj = nullptr;                tData  = nullptr;            }            else            {                --(refObj->ref);            }        }                tData = p.tData;        refObj = p.refObj;        p.tData = NULL;        p.refObj = NULL;        return *this;    };    template <typename U> SharedPtr &operator=(SharedPtr<U> &&p)    {        //printf("%s\n",__PRETTY_FUNCTION__);        if (refObj != NULL)        {            if (refObj->ref == 1)            {                refObj->ref = 0;                delete refObj;                refObj = nullptr;                tData  = nullptr;            }            else            {                --(refObj->ref);            }        }                tData = (T*)p.tData;        refObj = (ReferenceObject<T>*)p.refObj;        p.tData = nullptr;        p.refObj = nullptr;        return *this;    };        //DESTRUCTOR    //     Decrement count of managed OBJ.    //     IF reference count is 0 delete the object    ~SharedPtr()    {        //printf("%s\n", __PRETTY_FUNCTION__);        if (refObj != NULL)        {            if (refObj->ref == 1)            {                refObj->ref = 0;                delete refObj;                refObj = nullptr;                tData = nullptr;            }            else                --(refObj->ref);        }    };        //================================================    //================ MODIFIERS =====================    //================================================        void reset()    {        if (refObj != NULL)        {            if (refObj->ref == 1)            {                refObj->ref = 0;                delete refObj;                tData = nullptr;                refObj = nullptr;            }            else            {                --(refObj->ref);                tData = nullptr;                refObj = nullptr;            }        }    };        template <typename U> void reset(U *p)    {        if (refObj != NULL)        {            if (refObj->ref == 1)            {                refObj->ref = 0;                delete refObj;                tData = nullptr;                refObj = nullptr;            }            else            {                --(refObj->ref);                tData = nullptr;                refObj = nullptr;            }        }        refObj = new ReferenceObject<U>(p);        tData = (T*) (p);    };        //================================================    //================ OBSERVERS =====================    //================================================    T *get() const    {        return tData;    };    T &operator*() const    {        return *tData;    };    T *operator->() const    {        return tData;    };    explicit operator bool() const    {        if (tData != NULL)        {            return true;        }        return false;    };        };    //================================================    //================ NON-MEMBER ====================    //================================================    template <typename T1, typename T2>    bool operator==(const SharedPtr<T1> &p, const SharedPtr<T2> &p1)    {        if (p.tData == nullptr)        {            if (p1.tData == nullptr)            {                return true;            }            return false;        }        else if (p1.tData == nullptr)        {            return false;        }        else if (p.tData == p1.tData)        {            return true;        }        return false;    };        template <typename T>    bool operator==(const SharedPtr<T> &p, std::nullptr_t)    {        if (p == nullptr)        {            return true;        }        return false;    };    template <typename T>    bool operator==(std::nullptr_t, const SharedPtr<T> &p)    {        if (p == nullptr)        {            return true;        }        return false;    };        template <typename T1, typename T2>    bool operator!=(const SharedPtr<T1>& p, const SharedPtr<T2> &p1)    {        if (p.tData == NULL)        {            if (p1.tData == NULL)            {                return false;            }            return true;        }        else if (p.tData == p1.tData)        {            return false;        }        return true;    };        template <typename T>    bool operator!=(const SharedPtr<T> &p, std::nullptr_t)    {        if (p == nullptr)        {            return false;        }        return true;    };    template <typename T>    bool operator!=(std::nullptr_t, const SharedPtr<T> &p)    {        if (p == nullptr)        {            return false;        }        return true;    };        template <typename T, typename U>    SharedPtr<T> static_pointer_cast(const SharedPtr<U> &sp)    {        SharedPtr<T> temp(sp);        temp.tData = static_cast<T*>(sp.tData);        return temp;    };        template <typename T, typename U>    SharedPtr<T> dynamic_pointer_cast(const SharedPtr<U> &sp)    {        SharedPtr<T> temp(sp);        temp.tData = dynamic_cast<T*>(sp.tData);        return temp;    };    };#endif