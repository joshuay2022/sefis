OBJS = \
  $O\StdAfx.obj \
  $(CURRENT_OBJS) \
  $(COMPRESS_OBJS) \
  $(CRYPTO_OBJS) \
  $(STRING_OBJS) \
  $(PRINT_OBJS) \
  $(SSL_ODJS) \
  $(COMMON_OBJS)  \
  $(WIN_OBJS) \
  $(7ZIP_COMMON_OBJS) \

!include "./Build.mak"

!IFDEF MAK_SINGLE_FILE

!IFDEF CURRENT_OBJS
$(CURRENT_OBJS): ./$(*B).cpp
	$(COMPL)
!ENDIF

!IFDEF COMPRESS_OBJS
$(COMPRESS_OBJS): ./Compress/$(*B).cpp
!ENDIF

!IFDEF CRYPTO_OBJS
$(CRYPTO_OBJS): ./Crypto/$(*B).cpp
!ENDIF

!IFDEF STRING_OBJS
$(CONVERTSTRING_OBJS): ./String/$(*B).cpp
	$(COMPL)
!ENDIF

!IFDEF PRINT_OBJS
$(PRINT_OBJS): ./Print/$(*B).cpp
	$(COMPL)
!ENDIF

!IFDEF SSL_ODJS
$(OPENSSL_ODJS): ./SSL/$(*B).cpp
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

!IFDEF C_OBJS
$(C_OBJS): ../7zip/C/$(*B).c
	$(COMPL_O2)
!ENDIF

!ELSE

{.}.cpp{$O}.obj::
	$(COMPLB)
{./Compress}.cpp{$O}.obj::
	$(COMPLB)
{./Crypto}.cpp{$O}.obj::
	$(COMPLB)
{./String}.cpp{$O}.obj::
	$(COMPLB)
{./Print}.cpp{$O}.obj::
	$(COMPLB)
{./SSL}.cpp{$O}.obj::
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