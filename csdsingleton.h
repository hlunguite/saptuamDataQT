#ifndef CSDSINGLETON
#define CSDSINGLETON

template<class T>
class CsdSingleton {
public:
    static T* Object();
    static void Destroy();
private:
   static T* m_instance;
};

template<class T>
T* CsdSingleton<T>::m_instance = nullptr;

template<class T>
inline T*
CsdSingleton<T>::Object()
{
    if (m_instance == nullptr) {
        m_instance = new T;
    }
    return m_instance;
}

template<class T>
inline void
CsdSingleton<T>::Destroy()
{
    delete m_instance;
    m_instance = nullptr;
}

#endif // CSDSINGLETON

