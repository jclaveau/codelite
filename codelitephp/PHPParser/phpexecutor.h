#ifndef PHPEXECUTOR_H
#define PHPEXECUTOR_H

#include <wx/event.h>
#include <wx/process.h>
#include "php_project.h"
#include "TerminalEmulator.h"

class PHPExecutor : public wxEvtHandler
{
    TerminalEmulator m_terminal;

protected:
    bool RunRUL(PHPProject::Ptr_t pProject, const wxString &urlToRun, const wxString& xdebugSessionName);
    bool
    DoRunCLI(const wxString& script, PHPProject::Ptr_t proj, const wxString& xdebugSessionName, bool neverPauseOnExit);
    wxString DoGetCLICommand(const wxString& script, PHPProject::Ptr_t proj, wxString& errmsg);

public:
    PHPExecutor();
    virtual ~PHPExecutor();

    /**
     * @brief execute project. This function takes into account the project settings
     * @param projectName the project name
     * @param xdebugSessionName if debugging, use this xdebug-session name
     * @param neverPauseOnExit should we display a console with message 'Hit any key?'
     * @return true on success, false otherwise
     */
    bool Exec(const wxString& projectName,
              const wxString& urlOrFilePath,
              const wxString& xdebugSessionName,
              bool neverPauseOnExit);
    /**
     * @brief return true if a script is currently being executed using this instance
     */
    bool IsRunning() const;
    /**
     * @brief stop any executed php script
     */
    void Stop();

    /**
     * @brief run simple script using the global settings only and return its output
     */
    bool RunScript(const wxString& script, wxString& php_output);

    TerminalEmulator* GetTerminalEmulator() { return &m_terminal; }
};

#endif // PHPEXECUTOR_H
