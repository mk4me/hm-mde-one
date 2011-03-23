#include <utils/NamedTree.h>
#include <utils/Debug.h>

const NamedTreeBase::Separator NamedTreeBase::separator("/");

NamedTreeBase::NamedTreeBase(const std::string & name)
    : name(name)
{

}

NamedTreeBase::NamedTreeBase(const NamedTreeBase & child, bool deep)
    : name(child.name)
{
    //zawsze po kopii mamy roota
    if(deep == false){
        for(auto it = child.children.begin(); it != child.children.end(); it++){            
            //plytkie kopiowanie
            addChild(*it);
        }
    }else{
        for(auto it = child.children.begin(); it != child.children.end(); it++){
            //glebokie kopiowanie
            NamedTreeBasePtr newChild(new NamedTreeBase(**it,true));
            addChild(newChild);
        }        
    }
}

NamedTreeBase::NamedTreeBase(const NamedTreeBasePtr & parent, const std::string & name)
    : parent(parent), name(name)
{
    UTILS_ASSERT((parent != nullptr), "Nieprawidlowy rodzic aktualnego wezla");
}

NamedTreeBase::~NamedTreeBase(void)
{

}

//NamedTreeBasePtr NamedTreeBase::copy() const
//{
//    return NamedTreeBasePtr(new NamedTreeBase(this->getName()));
//}
//
//NamedTreeBasePtr NamedTreeBase::clone() const
//{
//    NamedTreeBasePtr ret(copy());
//
//    auto it = begin();
//    while(it != end()) {
//        ret->addChild((*it)->clone());
//        it++;
//    }
//
//    return ret;
//}

NamedTreeBasePtr NamedTreeBase::findChildByPath(const std::string & path) const
{
    UTILS_ASSERT((path.empty() == false), "Bledna sciezka do dziecka!");

    Tokenizer tok(path, separator);

    auto it = tok.begin();

    NamedTreeBasePtr ret = findLastChildInPath(it, tok.end());

    if(it != tok.end()) {
        ret.reset();
    }

    return ret;
}

const std::string & NamedTreeBase::getName() const
{ 
    return name;
}

void NamedTreeBase::setName(std::string name) 
{ 
    this->name = name; 
}


const NamedTreeBaseWPtr & NamedTreeBase::getParent()
{ 
    return this->parent;
}

const NamedTreeBaseConstWPtr & NamedTreeBase::getParent() const
{ 
    return this->constParent;
}

NamedTreeBase::iterator NamedTreeBase::begin()
{
    return children.begin();
}

NamedTreeBase::iterator NamedTreeBase::end()
{
    return children.end();
}

const NamedTreeBasePtr & NamedTreeBase::getChild(NamedTreeBase::size_type idx)
{
    return children[idx];
}

const NamedTreeBasePtr & NamedTreeBase::getChild(const std::string & path)
{
    NamedTreeBasePtr child = findChildByPath(path);
    if(child == nullptr) {
        throw std::runtime_error("Child does not exist!");
    }

    if(child->isRoot() == true){
        throw std::runtime_error("Root reference must be hold by client!");
    }

    NamedTreeBasePtr parent(child->getParent().lock());

    return *(parent->findChildByName(child->getName()));
}

NamedTreeBase::const_iterator NamedTreeBase::begin() const
{
    return constChildren.begin();
}

NamedTreeBase::const_iterator NamedTreeBase::end() const
{
    return constChildren.end();
}

const NamedTreeBaseConstPtr & NamedTreeBase::getChild(NamedTreeBase::size_type idx) const
{
    return constChildren[idx];
}

const NamedTreeBaseConstPtr & NamedTreeBase::getChild(const std::string & path) const
{
    NamedTreeBaseConstPtr child = findChildByPath(path);
    if(child == nullptr) {
        throw std::runtime_error("Child does not exist!");
    }

    if(child->isRoot() == true){
        throw std::runtime_error("Root reference must be hold by client!");
    }

    NamedTreeBaseConstPtr parent(child->getParent());

    return *(parent->findChildByName(child->getName()));
}

NamedTreeBase::size_type NamedTreeBase::size() const
{
    return children.size();
}

NamedTreeBase::iterator NamedTreeBase::findChildByName(const std::string & name)
{
    return findChildByName(children.begin(), children.end(), name);
}

NamedTreeBase::const_iterator NamedTreeBase::findChildByName(const std::string & name) const
{
    return findChildByName(constChildren.begin(), constChildren.end(), name);
}


void NamedTreeBase::addChild(const NamedTreeBasePtr & child)
{
    addChild(child, -1);
}

void NamedTreeBase::addChild(const std::string & path)
{
    addChild(path, -1);
}

void NamedTreeBase::addChild(const std::string & path, NamedTreeBase::size_type idx)
{
    UTILS_ASSERT((path.empty() == false), "Bledna sciezka do dziecka!");

    Tokenizer tok(path, separator);

    auto it = tok.begin();

    NamedTreeBasePtr pos = findLastChildInPath(it, tok.end());
    
    if(it == tok.end()) {
        throw std::runtime_error("Child with the given name already exist!");
    }

    ////wyznaczam ostatni poziom, bedacy nazwa dla mojego nowego wezla
    auto itEnd = tok.begin();
    auto itTmp = tok.begin();
    while(++itTmp != tok.end()){
        itEnd++;
    }

    //stworz drzewo do itEnd, a pod itEnd mamy juz nazwe naszego wezsla
    while(it != itEnd){
        NamedTreeBasePtr nodeChild(new NamedTreeBase(*it));
        pos->addChild(nodeChild);
        pos = nodeChild;
        it++;
    }

    pos->addChild(NamedTreeBasePtr(new NamedTreeBase(*itEnd)), idx);
}

void NamedTreeBase::addChild(const NamedTreeBasePtr & child, NamedTreeBase::size_type idx)
{
    if(child->isRoot() == false && child->getParent().lock() != nullptr) {
        throw std::runtime_error("Child has already a parent!");
    }

    if(std::find(children.begin(), children.end(), child) != children.end()) {
        throw std::runtime_error("Child already exist!");
    }

    if(findChildByName(child->getName()) != children.end()) {
        throw std::runtime_error("Child with the given name already exist!");
    }

    child->setParent(shared_from_this());
    if(idx == size_type(-1)){
        children.push_back(child);
        constChildren.push_back(child);
    }else {
        children[idx] = child;
        constChildren[idx] = child;
    }
}

void NamedTreeBase::removeChild(const NamedTreeBasePtr & child)
{
    auto it = std::remove( begin(), end(), child );
    if ( it != end() ) {
        //resetuj rodzica dziecka
        //moge to zrobic, gdyz jest jeszcze minimum jedna referencja do tego obiektu w const children!
        child->resetParent();
        children.resize( std::distance(begin(), it) );
        // usun element z dzieci const
        constChildren.resize(std::distance(constChildren.begin(), std::remove( constChildren.begin(), constChildren.end(), child)));
    }else{
        throw std::runtime_error("Child does not exist!"); 
    }
}

void NamedTreeBase::removeChild(NamedTreeBase::size_type idx)
{
    auto constIt = constChildren.begin();
    std::advance(constIt, idx);

    constChildren.erase(constIt);
    
    auto it = begin();
    std::advance(it, idx);
    (*it)->resetParent();
    children.erase(it);
}

void NamedTreeBase::removeChild(const std::string & path)
{
    UTILS_ASSERT((path.empty() == false), "Bledna sciezka do dziecka!");

    Tokenizer tok(path, separator);

    auto it = tok.begin();

    NamedTreeBasePtr pos = findLastChildInPath(it, tok.end());

    if(it != tok.end()) {
        throw std::runtime_error("Given path not exist!");
    }

    if(pos->isRoot() == true){
        throw std::runtime_error("Can not delete root - clinet code must do this explicitly!");
    }

    pos->getParent().lock()->removeChild(pos);
}

NamedTreeBaseWPtr NamedTreeBase::getRoot()
{    
    NamedTreeBaseWPtr ret(shared_from_this());

    while(ret.lock() != nullptr && ret.lock()->isRoot() == false){
        ret = ret.lock()->getParent().lock();
    }

    return ret;
}

NamedTreeBaseConstWPtr NamedTreeBase::getRoot() const
{    
    NamedTreeBaseConstWPtr ret(shared_from_this());

    while(ret.lock() != nullptr && ret.lock()->isRoot() == false){
        ret = ret.lock()->getParent().lock();
    }

    return ret;
}

bool NamedTreeBase::isLeaf() const
{
    return begin() == end();
}

bool NamedTreeBase::isRoot() const
{
    return getParent().lock() == nullptr;
}


void NamedTreeBase::setParent(const NamedTreeBasePtr & parent) 
{ 
    this->parent = parent; 
    this->constParent = parent;
}

void NamedTreeBase::resetParent()
{
    setParent(NamedTreeBasePtr());
}
