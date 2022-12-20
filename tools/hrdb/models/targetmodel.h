#ifndef TARGET_MODEL_H
#define TARGET_MODEL_H

#include <stdint.h>
#include <QObject>

#include "transport/remotecommand.h"
#include "breakpoint.h"
#include "memory.h"
#include "symboltable.h"
#include "registers.h"
#include "exceptionmask.h"
#include "../hardware/hardware_st.h"

class QTimer;
class ProfileData;
struct ProfileDelta;

class TargetChangedFlags
{
public:
    enum ChangedState
    {
        kPC,
        kRegs,
        kBreakpoints,
        kSymbolTable,
        kExceptionMask,
        kOtherMemory,        // e.g. a control set memory elsewhere
        kChangedStateCount
    };

    void Clear();
    void SetChanged(ChangedState ch) { m_changed[ch] = true; }
    void SetMemoryChanged(MemorySlot slot) { m_memChanged[slot] = true; }

    bool    m_changed[kChangedStateCount];
    bool    m_memChanged[kMemorySlotCount];
};

/*
    Simple container for YM register state
*/
class YmState
{
public:
    YmState();
    static const int kNumRegs = 16;
    void Clear();
    uint8_t m_regs[kNumRegs];
};

/*
    Core central data model reflecting the state of the target.
*/
class TargetModel : public QObject
{
	Q_OBJECT
public:
	TargetModel();
	virtual ~TargetModel();

    // These are called by the Dispatcher when notifications/events arrive
    void SetConnected(int running);
    void SetStatus(bool running, uint32_t pc, bool ffwd);
    void SetConfig(uint32_t machineType, uint32_t cpuLevel);

    // These are called by the Dispatcher when responses arrive
    void SetRegisters(const Registers& regs, uint64_t commandId);
    void SetMemory(MemorySlot slot, const Memory* pMem, uint64_t commandId);
    void SetBreakpoints(const Breakpoints& bps, uint64_t commandId);

    // Set Hatari's subtable of symbols
    void SetSymbolTable(const SymbolSubTable& syms, uint64_t commandId);
    void SetExceptionMask(const ExceptionMask& mask);
    void SetYm(const YmState& state);
    void NotifyMemoryChanged(uint32_t address, uint32_t size);

    // Update profiling data
    void AddProfileDelta(const ProfileDelta& delta);
    void ProfileDeltaComplete(int enabled);

    // (Called from UI)
    void ProfileReset();

    // User-added console command. Anything can happen!
    void ConsoleCommand();

    // User-inserted dummy command to signal e.g. end of a series of updates
    void Flush(uint64_t commmandId);

	// NOTE: all these return copies to avoid data contention
    MACHINETYPE	GetMachineType() const { return m_machineType; }

    int IsConnected() const { return m_bConnected; }
    int IsRunning() const { return m_bRunning; }
    int IsFastForward() const { return m_ffwd; }
    int IsProfileEnabled() const { return m_bProfileEnabled; }

    // This is the PC from start/stop notifications, so it's not valid when
    // running
    uint32_t GetStartStopPC() const { return m_startStopPc; }
	Registers GetRegs() const { return m_regs; }
    const Memory* GetMemory(MemorySlot slot) const
    {
        return m_pMemory[slot];
    }
    const Breakpoints& GetBreakpoints() const { return m_breakpoints; }
    const SymbolTable& GetSymbolTable() const { return m_symbolTable; }
    const ExceptionMask& GetExceptionMask() const { return m_exceptionMask; }
    YmState GetYm() const { return m_ymState; }

    // Profiling access
    void GetProfileData(uint32_t addr, uint32_t& count, uint32_t& cycles) const;
    const ProfileData& GetRawProfileData() const;

public slots:

signals:
    // connect/disconnect change
    void connectChangedSignal();

    // When start/stop status is changed
    void startStopChangedSignal();

    void startStopChangedSignalDelayed(int running);

    // When running, and the periodic refresh timer signals
    void runningRefreshTimerSignal();

    // When a user-inserted flush is the next command
    void flushSignal(const TargetChangedFlags& flags, uint64_t uid);

	// When new CPU registers are changed
    void registersChangedSignal(uint64_t commandId);

	// When a block of fetched memory is changed
	// TODO: don't have every view listening to the same slot?
    void memoryChangedSignal(int memorySlot, uint64_t commandId);

    void breakpointsChangedSignal(uint64_t commandId);
    void symbolTableChangedSignal(uint64_t commandId);
    void exceptionMaskChanged();
    void ymChangedSignal();

    // Something edited memory
    void otherMemoryChangedSignal(uint32_t address, uint32_t size);

    // Profile data changed
    void profileChangedSignal();
private slots:

    // Called shortly after stop notification received
    void delayedTimer();
    void runningRefreshTimerSlot();

private:
    TargetChangedFlags  m_changedFlags;

    MACHINETYPE     m_machineType;	// Hatari MACHINETYPE enum
    uint32_t        m_cpuLevel;		// CPU 0=000, 1=010, 2=020, 3=030, 4=040, 5=060

    int             m_bConnected;   // 0 == disconnected, 1 == connected
    int             m_bRunning;		// 0 == stopped, 1 == running
    int             m_bProfileEnabled; // 0 == off, 1 == collecting
    uint32_t        m_startStopPc;	// PC register (for next instruction)
    int             m_ffwd;         // 0 == normal, 1 == fast forward mode

    Registers       m_regs;			// Current register values
    Breakpoints     m_breakpoints;  // Current breakpoint list
    SymbolTable     m_symbolTable;
    ExceptionMask   m_exceptionMask;
    YmState         m_ymState;
    ProfileData*    m_pProfileData;

    // Actual current memory contents
    const Memory*   m_pMemory[MemorySlot::kMemorySlotCount];

    // Timer running to trigger events after CPU has stopped for a while
    // (e.g. Graphics Inspector refresh)
    QTimer*         m_pDelayedUpdateTimer;

    // Timer running to support refresh while running
    QTimer*         m_pRunningRefreshTimer;
};

// Helper functions to check broad machine types
extern bool IsMachineST(MACHINETYPE type);
extern bool IsMachineSTE(MACHINETYPE type);

#endif // TARGET_MODEL_H
