/********************************************************************
	created:  2010/08/05
	created:  5:8:2010   11:27
	filename: Profiler.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__UTILS_PROFILER_H__
#define __HEADER_GUARD__UTILS_PROFILER_H__

#include <map>
#include <string>
#include <ostream>
#include <vector>
#include <utils/Macros.h>
#include <utils/System.h>
#include <time.h>

#if defined(WIN32)
#include <windows.h>
#elif defined(__GNUC__)
#else
#error Unknown compiler.
#endif

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

struct ProfilerEntry;
struct ProfilerEntryUpdater;
struct ProfilerThreadScope;
struct ThreadEntryToCSV;

//! Maksymalna liczba wpis�w.
#define UTILS_PROFILER_MAX_ENTRIES 1000
//! Maksymalna liczba aktywnych w�tk�w mo�liwa do identyfikacji bez rozga��zie�.
#define UTILS_PROFILER_LIST_SIZE 4
//! Domy�lny plik docelowy.
#define UTILS_PROFILER_DEFAULT_DUMP_FILE "ProfilerLog.csv"

/**
 *  Profiler wielow�tkowy. Dzia�a z minimalnymi narzutami (kilka promili) nawet
 *  w trybie debug.
 *  �eby uaktywni� profilowanie w�tku, nale�y albo stworzy� lokalnie instancj�
 *  typu ThreadScope, albo pos�u�y� si� metod� Profiler::setThreadEnabled
 *  przy rozpoczynaniu i ko�czeniu w�tku.
 *  Pomiary odbywaj� si� przy u�yciu statycznych wpis�w identyfikuj�cych funkcj�
 *  (ProfilerEntry) oraz ich aktualizatory, tworzone lokalnie ProfilerEntryUpdater. 
 *  Gdy zasi�g nazw, dla kt�rego stworzono instancj� ProfilerEntryUpdater si� zako�czy,
 *  destruktur na podstawie wska�nika do ProfilerEntry b�dzie w stanie zaktualizowa�
 *  dane pomiarowe.
 *
 *  Problemem okaza�o si� przeprowadzanie pomiar�w �rodowisku wielow�tkowym. Ostatecznie
 *  ka�dy w�tek dysponuje swoim zestawem pomiar�w, dzi�ki czemu kod synchronizuj�cy
 *  ograniczony jest do minimum. W momencie wykonywania zrzutu do pliku dane s� konsolidowane.
 *  Dodatkowo je�eli liczba aktywnych w�tk�w jest niska (<= UTILS_PROFILER_LIST_SIZE),
 *  wyznaczenie zestawu danych na podsatwie id odbywa si� bez instrukcji rozga��zie�. Je�eli
 *  liczba aktywnych w�tk�w uro�nie ponad t� liczb�, w�wczas u�ywany jest kontener std::map,
 *  w kt�rym overhead zwi�zany z obs�ug� drzewa zwi�ksza narzut czasowy Profilera, zw�aszcza
 *  w trybie Debug.
 */
class Profiler
{
  friend struct ProfilerEntry;
  friend struct ProfilerEntryUpdater;
  friend struct ThreadScope;
  friend struct ThreadEntryToCSV;

public:
  //! Czas w wysokiej rozdzielczo�ci.
  typedef __system::Tick tick;
  //! Typ id w�tku.
  typedef __system::ThreadID threadID;

  //! Typ w��czaj�cy profilowanie bie��cego w�tku w bie��cej przestrzeni nazw.
  struct ThreadScope
  {
    ThreadScope()  { Profiler::instance.setThreadEnabled(true);}
    ~ThreadScope() { Profiler::instance.setThreadEnabled(false); }
  };

  //! Pomiar.
  struct Measurement
  {
    //! Liczba tykni��.
    Profiler::tick ticks;
    //! Liczba wej��.
    unsigned entrances;
  };

  //! Stan profilingu.
  struct State
  {
    //! Bie��cy wpis.
    ProfilerEntry* currentEntry;
    //! Czas rozpocz�cia.
    tick start;
    //! ��czny czas trwania.
    tick duration;
    //! Pomiary.
    Measurement measurements[UTILS_PROFILER_MAX_ENTRIES];
  };

  //! Skojarzenie w�tku ze stanem.
  typedef std::pair<threadID, State*> ThreadEntry;
  //! Dane o w�tkach w postaci mapy.
  typedef std::map<ThreadEntry::first_type, ThreadEntry::second_type> ThreadMap;

private:
  //! Instancja profilera.
  static Profiler instance;
  //! ID nast�pnego wpisu.
  volatile long nextID;
  //! Czy jest w��czony?
  bool enabled;
  //! Czas trwania programu.
  tick duration;
  //! Czas startu programu.
  tick start;
  //! Czas ostatniego w��czenia.
  tick enabledStart;
  //! ��czny czas w��czenia.
  tick enabledDuration;
  //! Czas zmarnotrawiony na logik� profilera.
  volatile tick overhead;
  //! Wpisy.
  ProfilerEntry* entires[UTILS_PROFILER_MAX_ENTRIES];
  //! Lista aktywnych w�tk�w.
  ThreadEntry threadList[UTILS_PROFILER_LIST_SIZE];
  //! Mapa aktywnych w�tk�w.
  ThreadMap threadMap;
  //! Liczba aktywnych w�tk�w.
  int threadCount;
  //! Niekatywne ju� w�tki.
  ThreadMap inactiveThreads;

  //! Plik do automatycznego zapisu loga.
  std::string autoDumpFile;
  //! Czy w logu dane pomiarowe maj� by� rozbite na poszczegolne w�tki?
  bool perThreadDumpEnabled;

public:
  //! 
  Profiler();
  //! 
  ~Profiler();

public:

  //! \return Referencja do instancji.
  static Profiler& getInstance()
  {
    return instance;
  }


  //! Czy profiler jest w��czony?
  //! \param enabled
  static void setEnabled(bool enabled);
  //! Czy profilowanie dla w�tku jest w��czone?
  void setThreadEnabled(bool enabled);
  //! Czy profilowanie dla w�tku jest w��czone?
  bool isThreadEnabled();

  //! \return �cie�ka do automatycznego zapisu.
  static const std::string& getAutoDumpFile() 
  { 
    return instance.autoDumpFile;
  }
  //! \param autoDumpFile �cie�ka do automatycznego zapisu.
  static void setAutoDumpFile(const std::string& autoDumpFile) 
  { 
    instance.autoDumpFile = autoDumpFile; 
  }

  //! \return Tykni�cia zegara o wysokiej cz�stotliwo�ci.
  inline static tick getTick()
  {
    return __system::getTick();
  }
  //! Konwersja tykni�cia do sekund.
  inline static double tickToSeconds(tick value)
  {
    return __system::tickToSeconds(value);
  }

  //! Zapisuje log pod zadany strumie�.
  static void dump(std::ostream& output);
  //! \return Log.
  static std::string dump();

  //! \return
  static bool isPerThreadDumpEnabled()
  { 
    return instance.perThreadDumpEnabled;
  }
  //! \param perThreadDumpEnabled
  static void setPerThreadDumpEnabled(bool perThreadDumpEnabled) 
  { 
    instance.perThreadDumpEnabled = perThreadDumpEnabled; 
  }

private:
  //! Dodaje wpis do listy.
  static void addEntry(ProfilerEntry * entry);
  
  //! \return Stan dla bie��cego w�tku b�d� NULL, je�eli jest wy��czony.
  State* getState();

  //! W��cza profilowanie dla w�tku.
  void enableThread(threadID thread);
  //! Wy��cza profileowanie dla w�tku.
  void disableThread(threadID thread);

  //! Zrzuca dane do �a�cucha.
  //! \param output
  std::string createCSV();
  //! Funkcja zrzucaj�ca pewien przedzia� element�w ThreadEntry

  template <class FwdIter>
  void scopeToCSV(std::ostringstream& str, FwdIter first, FwdIter last);

  //! Funkcja wykonuj�ca jak�� czynno�� na wszystkich stanach wszystkich w�tk�w.
  template <class Func>
  void forEachThread(Func func);

  //!
  void stateToCSV(const State& state, std::ostream& output, const threadID* id);
};




/**
 *	Wpis profilera. Typ przeznaczany do tworzenia jako zmienne statyczne w funckjach. Rodzi
 *  to problem wy�cigu w warunkach wielow�tkowo�ci, ale poniewa� typ nie konstruuje �adnych
 *  podobiekt�w ani ich nie kasuje - nie jest wa�ne, ile razy b�dzie wywo�any
 *  konstruktor / destruktor.
 */
struct ProfilerEntry
{
  enum {
    InvalidID = -1
  };

  //! ID wpisu.
  long id;
  //! Sygnatura funkcji.
  const char* signature;
  //! Nazwa funkcji.
  const char* function;
  //! Plik.
  const char* file;
  //! Linia pliku.
  unsigned line;

  //! 
  //! \param signature
  //! \param function
  //! \param file
  //! \param line
  ProfilerEntry(const char * signature, const char * function, const char * file, unsigned line);

  //!
  ~ProfilerEntry();

  //! \param length D�ugo�� nazwy.
  //! \return Nazwa typu. Nie jest zako�czona zerem!
  const char* getTypeName(int& length) const;
  //! \return Typ.
  const char* getFunctionName() const;
};


/**
 *	Struktura aktualizuj�ca dane profilera.
 */
struct ProfilerEntryUpdater
{
  //! Poprzedni wpis.
  ProfilerEntry* prevEntry;
  //! Pomiary.
  Profiler::State* state;
  //! Pomiary.
  Profiler::Measurement* measurement;
  //! Poprzedni czas wpisu minus tick wej�cia. Czemu po prostu nie dodajemy delty do czasu? Wywo�ania rekurencyjne!
  Profiler::tick entryTickMinusPrev;
  //! Czy w��czony?
  bool enabled;

  //! 
  //! \param entry
  ProfilerEntryUpdater(ProfilerEntry * entry)
  {
    if ( state = Profiler::instance.getState() ) {
      // zamiana bie��cego wpisu
      prevEntry = state->currentEntry;
      state->currentEntry = entry;
      measurement = state->measurements + entry->id;
      // aktualizacja i zapami�tanie czasu wej�cia
      if ( enabled = Profiler::instance.enabled ) {
        ++measurement->entrances;
        entryTickMinusPrev = Profiler::getTick() - measurement->ticks;
      }
    } 
  }

  //! 
  ~ProfilerEntryUpdater()
  {
    if ( state ) {
      if ( enabled ) {
        measurement->ticks = Profiler::getTick() - entryTickMinusPrev;
      }
      state->currentEntry = prevEntry;
    }
  }
};

//#define DISABLE_PROFILER
//#define UTILS_RELEASE_PROFILER
#if !defined(DISABLE_PROFILER) && (defined(_DEBUG) || defined(DEBUG) || defined(UTILS_RELEASE_PROFILER))

//! Makro, kt�re nale�y wstawi� w g��wnej funkcji w�tku, aby umo�liwi� zbieranie danych
//! pomiarowych z nim zwi�zanych
#define UTILS_PROFILER_THREAD_SCOPE utils::Profiler::ThreadScope __profiler_thread_scope;

//! Makro s�u��ce do pomiaru czasu wykonania funkcji. Powinno by� umieszczane
//! w pierwszej linii. NIE nale�y umieszcza� w funkcjach inline (gdy� przestan�
//! wtedy by� inline :))
#if defined(WIN32)
#define UTILS_PROFILER_ENTRY \
  static utils::ProfilerEntry __profiler_entry(__FUNCSIG__, __FUNCTION__, __FILE__, __LINE__);\
  utils::ProfilerEntryUpdater __profiler_updater(&__profiler_entry)
#elif defined(__GNUC__)
#define UTILS_PROFILER_ENTRY \
  static utils::ProfilerEntry __profiler_entry(__PRETTY_FUNCTION__, __FUNCTION__, __FILE__, __LINE__);\
  utils::ProfilerEntryUpdater __profiler_updater(&__profiler_entry)
#endif

//! Makro s�u��ce do pomiaru cz�ci funkcji. Nazwa mo�e sk�ada� si� tylko ze znak�w
//! alfanumerycznych.
//! \param name Nazwa zakresu
#if defined(WIN32)
#define UTILS_PROFILER_SCOPE(name) \
  static utils::ProfilerEntry __scope_entry_##name(__FUNCSIG__, __FUNCTION__ "{" STRINGIZE(name) "}", __FILE__, __LINE__); \
  utils::ProfilerEntryUpdater __scope_updater_##name(&__scope_entry_##name)
#elif defined(__GNUC__)
// nie da si� robi� konkatenacji __FUNCTION__ i �a�cuch�w...
#define UTILS_PROFILER_SCOPE(name) \
  static utils::ProfilerEntry __scope_entry_##name(__PRETTY_FUNCTION__, "customscope::" STRINGIZE(name), __FILE__, __LINE__); \
  utils::ProfilerEntryUpdater __scope_updater_##name(&__scope_entry_##name)
#endif

#else

#define UTILS_PROFILER_THREAD_SCOPE
#define UTILS_PROFILER_ENTRY
#define UTILS_PROFILER_SCOPE(name)

#endif


//static utils::ProfilerEntry CONCATENATE(__profiler_entry, __LINE__)(__FUNCSIG__, __FUNCTION__ "!" name, __FILE__, __LINE__); utils::ProfilerEntryUpdater CONCATENATE(__profiler_updater, __LINE__)(&CONCATENATE(__profiler_entry, __LINE__));

////////////////////////////////////////////////////////////////////////////////
} // namespace utils
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__PROFILER_H__