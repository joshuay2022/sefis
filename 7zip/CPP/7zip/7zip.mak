OBJS = \
  $O\StdAfx.obj \
  $(CURRENT_OBJS) \
  $(COMMON_OBJS) \
  $(WIN_OBJS) \
  $(WIN_CTRL_OBJS) \
  $(7ZIP_COMMON_OBJS) \
  $(AR_OBJS) \
  $(AR_COMMON_OBJS) \
  $(UI_COMMON_OBJS) \
  $(AGENT_OBJS) \
  $(CONSOLE_OBJS) \
  $(EXPLORER_OBJS) \
  $(FM_OBJS) \
  $(GUI_OBJS) \
  $(7Z_OBJS) \
  $(CAB_OBJS) \
  $(CHM_OBJS) \
  $(COM_OBJS) \
  $(ISO_OBJS) \
  $(NSIS_OBJS) \
  $(RAR_OBJS) \
  $(TAR_OBJS) \
  $(UDF_OBJS) \
  $(WIM_OBJS) \
  $(ZIP_OBJS) \
  $(COMPRESS_OBJS) \
  $(CRYPTO_OBJS) \
  $(C_OBJS) \
  $(ASM_OBJS) \
#  $O\resource.res 

!include "../7zip/CPP/Build.mak"

# MAK_SINGLE_FILE = 1

!IFDEF MAK_SINGLE_FILE

!IFDEF CURRENT_OBJS
$(CURRENT_OBJS): ./$(*B).cpp
	$(COMPL)
!ENDIF


!IFDEF COMMON_OBJS
$(COMMON_OBJS): ../7zip/CPP/Common/$(*B).cpp
	$(COMPL)
!ENDIF

!IFDEF WIN_OBJS
$(WIN_OBJS): ../7zip/CPP/Windows/$(*B).cpp
	$(COMPL)
!ENDIF

!IFDEF WIN_CTRL_OBJS
$(WIN_CTRL_OBJS): ../7zip/CPP/Windows/Control/$(*B).cpp
	$(COMPL)
!ENDIF

!IFDEF 7ZIP_COMMON_OBJS
$(7ZIP_COMMON_OBJS): ../7zip/CPP/7zip/Common/$(*B).cpp
	$(COMPL)
!ENDIF

!IFDEF AR_OBJS
$(AR_OBJS): ../7zip/CPP/7zip/Archive/$(*B).cpp
	$(COMPL)
!ENDIF

!IFDEF AR_COMMON_OBJS
$(AR_COMMON_OBJS): ../7zip/CPP/7zip/Archive/Common/$(*B).cpp
	$(COMPL)
!ENDIF

!IFDEF 7Z_OBJS
$(7Z_OBJS): ../7zip/CPP/7zip/Archive/7z/$(*B).cpp
	$(COMPL)
!ENDIF

!IFDEF CAB_OBJS
$(CAB_OBJS): ../7zip/CPP/7zip/Archive/Cab/$(*B).cpp
	$(COMPL)
!ENDIF

!IFDEF CHM_OBJS
$(CHM_OBJS): ../7zip/CPP/7zip/Archive/Chm/$(*B).cpp
	$(COMPL)
!ENDIF

!IFDEF COM_OBJS
$(COM_OBJS): ../7zip/CPP/7zip/Archive/Com/$(*B).cpp
	$(COMPL)
!ENDIF

!IFDEF ISO_OBJS
$(ISO_OBJS): ../7zip/CPP/7zip/Archive/Iso/$(*B).cpp
	$(COMPL)
!ENDIF

!IFDEF NSIS_OBJS
$(NSIS_OBJS): ../7zip/CPP/7zip/Archive/Nsis/$(*B).cpp
	$(COMPL)
!ENDIF

!IFDEF RAR_OBJS
$(RAR_OBJS): ../7zip/CPP/7zip/Archive/Rar/$(*B).cpp
	$(COMPL)
!ENDIF

!IFDEF TAR_OBJS
$(TAR_OBJS): ../7zip/CPP/7zip/Archive/Tar/$(*B).cpp
	$(COMPL)
!ENDIF

!IFDEF UDF_OBJS
$(UDF_OBJS): ../7zip/CPP/7zip/Archive/Udf/$(*B).cpp
	$(COMPL)
!ENDIF

!IFDEF WIM_OBJS
$(WIM_OBJS): ../7zip/CPP/7zip/Archive/Wim/$(*B).cpp
	$(COMPL)
!ENDIF

!IFDEF ZIP_OBJS
$(ZIP_OBJS): ../7zip/CPP/7zip/Archive/Zip/$(*B).cpp
	$(COMPL)
!ENDIF

!IFDEF COMPRESS_OBJS
$(COMPRESS_OBJS): ../7zip/CPP/7zip/Compress/$(*B).cpp
	$(COMPL_O2)
!ENDIF

!IFDEF CRYPTO_OBJS
$(CRYPTO_OBJS): ../7zip/CPP/7zip/Crypto/$(*B).cpp
	$(COMPL_O2)
!ENDIF

!IFDEF UI_COMMON_OBJS
$(UI_COMMON_OBJS): ../7zip/CPP/7zip/UI/Common/$(*B).cpp
	$(COMPL)
!ENDIF

!IFDEF AGENT_OBJS
$(AGENT_OBJS): ../7zip/CPP/7zip/UI/Agent/$(*B).cpp
	$(COMPL)
!ENDIF

!IFDEF CONSOLE_OBJS
$(CONSOLE_OBJS): ../7zip/CPP/7zip/UI/Console/$(*B).cpp
	$(COMPL)
!ENDIF

!IFDEF EXPLORER_OBJS
$(EXPLORER_OBJS): ../7zip/CPP/7zip/UI/Explorer/$(*B).cpp
	$(COMPL)
!ENDIF

!IFDEF FM_OBJS
$(FM_OBJS): ../7zip/CPP/7zip/UI/FileManager/$(*B).cpp
	$(COMPL)
!ENDIF

!IFDEF GUI_OBJS
$(GUI_OBJS): ../7zip/CPP/7zip/UI/GUI/$(*B).cpp
	$(COMPL)
!ENDIF

!IFDEF C_OBJS
$(C_OBJS): ../7zip/C/$(*B).c
	$(COMPL_O2)
!ENDIF


!ELSE

{.}.cpp{$O}.obj::
	$(COMPLB)
{../7zip/CPP/Common}.cpp{$O}.obj::
	$(COMPLB)
{../7zip/CPP/Windows}.cpp{$O}.obj::
	$(COMPLB)
{../7zip/CPP/Windows/Control}.cpp{$O}.obj::
	$(COMPLB)
{../7zip/CPP/7zip/Common}.cpp{$O}.obj::
	$(COMPLB)

{../7zip/CPP/7zip/UI/Common}.cpp{$O}.obj::
	$(COMPLB)
{../7zip/CPP/7zip/UI/Agent}.cpp{$O}.obj::
	$(COMPLB)
{../7zip/CPP/7zip/UI/Console}.cpp{$O}.obj::
	$(COMPLB)
{../7zip/CPP/7zip/UI/Explorer}.cpp{$O}.obj::
	$(COMPLB)
{../7zip/CPP/7zip/UI/FileManager}.cpp{$O}.obj::
	$(COMPLB)
{../7zip/CPP/7zip/UI/GUI}.cpp{$O}.obj::
	$(COMPLB)


{../7zip/CPP/7zip/Archive}.cpp{$O}.obj::
	$(COMPLB)
{../7zip/CPP/7zip/Archive/Common}.cpp{$O}.obj::
	$(COMPLB)

{../7zip/CPP/7zip/Archive/7z}.cpp{$O}.obj::
	$(COMPLB)
{../7zip/CPP/7zip/Archive/Cab}.cpp{$O}.obj::
	$(COMPLB)
{../7zip/CPP/7zip/Archive/Chm}.cpp{$O}.obj::
	$(COMPLB)
{../7zip/CPP/7zip/Archive/Com}.cpp{$O}.obj::
	$(COMPLB)
{../7zip/CPP/7zip/Archive/Iso}.cpp{$O}.obj::
	$(COMPLB)
{../7zip/CPP/7zip/Archive/Nsis}.cpp{$O}.obj::
	$(COMPLB)
{../7zip/CPP/7zip/Archive/Rar}.cpp{$O}.obj::
	$(COMPLB)
{../7zip/CPP/7zip/Archive/Tar}.cpp{$O}.obj::
	$(COMPLB)
{../7zip/CPP/7zip/Archive/Udf}.cpp{$O}.obj::
	$(COMPLB)
{../7zip/CPP/7zip/Archive/Wim}.cpp{$O}.obj::
	$(COMPLB)
{../7zip/CPP/7zip/Archive/Zip}.cpp{$O}.obj::
	$(COMPLB)

{../7zip/CPP/7zip/Compress}.cpp{$O}.obj::
	$(COMPLB_O2)
{../7zip/CPP/7zip/Crypto}.cpp{$O}.obj::
	$(COMPLB_O2)
{../7zip/C}.c{$O}.obj::
	$(CCOMPLB)

!ENDIF

!include "Asm.mak"
