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

//! Maksymalna liczba wpisów.
#define UTILS_PROFILER_MAX_ENTRIES 1000
//! Maksymalna liczba aktywnych w¹tków mo¿liwa do identyfikacji bez rozga³êzieñ.
#define UTILS_PROFILER_LIST_SIZE 4
//! Domyœlny plik docelowy.
#define UTILS_PROFILER_DEFAULT_DUMP_FILE "ProfilerLog.csv"

/**
 *  Profiler wielow¹tkowy. Dzia³a z minimalnymi narzutami (kilka promili) nawet
 *  w trybie debug.
 *  ¯eby uaktywniæ profilowanie w¹tku, nale¿y albo stworzyæ lokalnie instancjê
 *  typu ThreadScope, albo pos³u¿yæ siê metod¹ Profiler::setThreadEnabled
 *  przy rozpoczynaniu i koñczeniu w¹tku.
 *  Pomiary odbywaj¹ siê przy u¿yciu statycznych wpisów identyfikuj¹cych funkcjê
 *  (ProfilerEntry) oraz ich aktualizatory, tworzone lokalnie ProfilerEntryUpdater. 
 *  Gdy zasiêg nazw, dla którego stworzono instancjê ProfilerEntryUpdater siê zakoñczy,
 *  destruktur na podstawie wskaŸnika do ProfilerEntry bêdzie w stanie zaktualizowaæ
 *  dane pomiarowe.
 *
 *  Problemem okaza³o siê przeprowadzanie pomiarów œrodowisku wielow¹tkowym. Ostatecznie
 *  ka¿dy w¹tek dysponuje swoim zestawem pomiarów, dziêki czemu kod synchronizuj¹cy
 *  ograniczony jest do minimum. W momencie wykonywania zrzutu do pliku dane s¹ konsolidowane.
 *  Dodatkowo je¿eli liczba aktywnych w¹tków jest niska (<= UTILS_PROFILER_LIST_SIZE),
 *  wyznaczenie zestawu danych na podsatwie id odbywa siê bez instrukcji rozga³êzieñ. Je¿eli
 *  liczba aktywnych w¹tków uroœnie ponad tê liczbê, wówczas u¿ywany jest kontener std::map,
 *  w którym overhead zwi¹zany z obs³ug¹ drzewa zwiêksza narzut czasowy Profilera, zw³aszcza
 *  w trybie Debug.
 */
class Profiler
{
  friend struct ProfilerEntry;
  friend struct ProfilerEntryUpdater;
  friend struct ThreadScope;
  friend struct ThreadEntryToCSV;

public:
  //! Czas w wysokiej rozdzielczoœci.
  typedef __system::Tick tick;
  //! Typ id w¹tku.
  typedef __system::ThreadID threadID;

  //! Typ w³¹czaj¹cy profilowanie bie¿¹cego w¹tku w bie¿¹cej przestrzeni nazw.
  struct ThreadScope
  {
    ThreadScope()  { Profiler::instance.setThreadEnabled(true);}
    ~ThreadScope() { Profiler::instance.setThreadEnabled(false); }
  };

  //! Pomiar.
  struct Measurement
  {
    //! Liczba tykniêæ.
    Profiler::tick ticks;
    //! Liczba wejœæ.
    unsigned entrances;
  };

  //! Stan profilingu.
  struct State
  {
    //! Bie¿¹cy wpis.
    ProfilerEntry* currentEntry;
    //! Czas rozpoczêcia.
    tick start;
    //! £¹czny czas trwania.
    tick duration;
    //! Pomiary.
    Measurement measurements[UTILS_PROFILER_MAX_ENTRIES];
  };

  //! Skojarzenie w¹tku ze stanem.
  typedef std::pair<threadID, State*> ThreadEntry;
  //! Dane o w¹tkach w postaci mapy.
  typedef std::map<ThreadEntry::first_type, ThreadEntry::second_type> ThreadMap;

private:
  //! Instancja profilera.
  static Profiler instance;
  //! ID nastêpnego wpisu.
  volatile long nextID;
  //! Czy jest w³¹czony?
  bool enabled;
  //! Czas trwania programu.
  tick duration;
  //! Czas startu programu.
  tick start;
  //! Czas ostatniego w³¹czenia.
  tick enabledStart;
  //! £¹czny czas w³¹czenia.
  tick enabledDuration;
  //! Czas zmarnotrawiony na logikê profilera.
  volatile tick overhead;
  //! Wpisy.
  ProfilerEntry* entires[UTILS_PROFILER_MAX_ENTRIES];
  //! Lista aktywnych w¹tków.
  ThreadEntry threadList[UTILS_PROFILER_LIST_SIZE];
  //! Mapa aktywnych w¹tków.
  ThreadMap threadMap;
  //! Liczba aktywnych w¹tków.
  int threadCount;
  //! Niekatywne ju¿ w¹tki.
  ThreadMap inactiveThreads;

  //! Plik do automatycznego zapisu loga.
  std::string autoDumpFile;
  //! Czy w logu dane pomiarowe maj¹ byæ rozbite na poszczegolne w¹tki?
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


  //! Czy profiler jest w³¹czony?
  //! \param enabled
  static void setEnabled(bool enabled);
  //! Czy profilowanie dla w¹tku jest w³¹czone?
  void setThreadEnabled(bool enabled);
  //! Czy profilowanie dla w¹tku jest w³¹czone?
  bool isThreadEnabled();

  //! \return Œcie¿ka do automatycznego zapisu.
  static const std::string& getAutoDumpFile() 
  { 
    return instance.autoDumpFile;
  }
  //! \param autoDumpFile Œcie¿ka do automatycznego zapisu.
  static void setAutoDumpFile(const std::string& autoDumpFile) 
  { 
    instance.autoDumpFile = autoDumpFile; 
  }

  //! \return Tykniêcia zegara o wysokiej czêstotliwoœci.
  inline static tick getTick()
  {
    return __system::getTick();
  }
  //! Konwersja tykniêcia do sekund.
  inline static double tickToSeconds(tick value)
  {
    return __system::tickToSeconds(value);
  }

  //! Zapisuje log pod zadany strumieñ.
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
  
  //! \return Stan dla bie¿¹cego w¹tku b¹dŸ NULL, je¿eli jest wy³¹czony.
  State* getState();

  //! W³¹cza profilowanie dla w¹tku.
  void enableThread(threadID thread);
  //! Wy³¹cza profileowanie dla w¹tku.
  void disableThread(threadID thread);

  //! Zrzuca dane do ³añcucha.
  //! \param output
  std::string createCSV();
  //! Funkcja zrzucaj¹ca pewien przedzia³ elementów ThreadEntry

  template <class FwdIter>
  void scopeToCSV(std::ostringstream& str, FwdIter first, FwdIter last);

  //! Funkcja wykonuj¹ca jak¹œ czynnoœæ na wszystkich stanach wszystkich w¹tków.
  template <class Func>
  void forEachThread(Func func);

  //!
  void stateToCSV(const State& state, std::ostream& output, const threadID* id);
};




/**
 *	Wpis profilera. Typ przeznaczany do tworzenia jako zmienne statyczne w funckjach. Rodzi
 *  to problem wyœcigu w warunkach wielow¹tkowoœci, ale poniewa¿ typ nie konstruuje ¿adnych
 *  podobiektów ani ich nie kasuje - nie jest wa¿ne, ile razy bêdzie wywo³any
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

  //! \param length D³ugoœæ nazwy.
  //! \return Nazwa typu. Nie jest zakoñczona zerem!
  const char* getTypeName(int& length) const;
  //! \return Typ.
  const char* getFunctionName() const;
};


/**
 *	Struktura aktualizuj¹ca dane profilera.
 */
struct ProfilerEntryUpdater
{
  //! Poprzedni wpis.
  ProfilerEntry* prevEntry;
  //! Pomiary.
  Profiler::State* state;
  //! Pomiary.
  Profiler::Measurement* measurement;
  //! Poprzedni czas wpisu minus tick wejœcia. Czemu po prostu nie dodajemy delty do czasu? Wywo³ania rekurencyjne!
  Profiler::tick entryTickMinusPrev;
  //! Czy w³¹czony?
  bool enabled;

  //! 
  //! \param entry
  ProfilerEntryUpdater(ProfilerEntry * entry)
  {
    if ( state = Profiler::instance.getState() ) {
      // zamiana bie¿¹cego wpisu
      prevEntry = state->currentEntry;
      state->currentEntry = entry;
      measurement = state->measurements + entry->id;
      // aktualizacja i zapamiêtanie czasu wejœcia
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

//! Makro, które nale¿y wstawiæ w g³ównej funkcji w¹tku, aby umo¿liwiæ zbieranie danych
//! pomiarowych z nim zwi¹zanych
#define UTILS_PROFILER_THREAD_SCOPE utils::Profiler::ThreadScope __profiler_thread_scope;

//! Makro s³u¿¹ce do pomiaru czasu wykonania funkcji. Powinno byæ umieszczane
//! w pierwszej linii. NIE nale¿y umieszczaæ w funkcjach inline (gdy¿ przestan¹
//! wtedy byæ inline :))
#if defined(WIN32)
#define UTILS_PROFILER_ENTRY \
  static utils::ProfilerEntry __profiler_entry(__FUNCSIG__, __FUNCTION__, __FILE__, __LINE__);\
  utils::ProfilerEntryUpdater __profiler_updater(&__profiler_entry)
#elif defined(__GNUC__)
#define UTILS_PROFILER_ENTRY \
  static utils::ProfilerEntry __profiler_entry(__PRETTY_FUNCTION__, __FUNCTION__, __FILE__, __LINE__);\
  utils::ProfilerEntryUpdater __profiler_updater(&__profiler_entry)
#endif

//! Makro s³u¿¹ce do pomiaru czêœci funkcji. Nazwa mo¿e sk³adaæ siê tylko ze znaków
//! alfanumerycznych.
//! \param name Nazwa zakresu
#if defined(WIN32)
#define UTILS_PROFILER_SCOPE(name) \
  static utils::ProfilerEntry __scope_entry_##name(__FUNCSIG__, __FUNCTION__ "{" STRINGIZE(name) "}", __FILE__, __LINE__); \
  utils::ProfilerEntryUpdater __scope_updater_##name(&__scope_entry_##name)
#elif defined(__GNUC__)
// nie da siê robiæ konkatenacji __FUNCTION__ i ³añcuchów...
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