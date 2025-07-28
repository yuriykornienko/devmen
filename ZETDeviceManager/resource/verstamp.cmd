::'@echo off
::':: Batch with embedded VBS ::
::'@cscript //nologo //e:vbscript "%~0" %*
::'@exit /b 0

'-----------------------------------------------------------------------------
On Error Resume Next

Dim build
build = "2013-04-05"

Dim fso
Set fso = CreateObject("Scripting.FileSystemObject")

'-----------------------------------------------------------------------------
Dim ver_y, ver_m, ver_d, ver_num, ver_str
ver_y = Year(Date)
ver_m = Month(Date)
ver_d = Day(Date)
ver_num = ver_y & "," & ver_m & "," & ver_d
ver_str = ver_y & "." & ver_m & "." & ver_d

'-----------------------------------------------------------------------------
Dim re_begin
Set re_begin = New RegExp
re_begin.Pattern = "VS_VERSION_INFO"

Dim re_vernum
Set re_vernum = New RegExp
re_vernum.Pattern = "^(\s*(FILEVERSION|PRODUCTVERSION)\s+?)([^,]+?,[^,]+?,[^,]+?)(,[^,]+?)$"

Dim re_verstr
Set re_verstr = New RegExp
re_verstr.Pattern = "^(\s*VALUE\s+""(FileVersion|ProductVersion)""\s*,\s+?"")([^.]+?\.[^.]+?\.[^.]+?)(\.[^.]+?)$"

Dim re_end
Set re_end = New RegExp
re_end.Pattern = "^END$"

'-----------------------------------------------------------------------------
Sub UpdateVersion(path_rc)
	Dim rc
	Set rc = fso.OpenTextFile(path_rc, 1, False)

	Dim test1, test2
	test1 = rc.Read(1)
	test2 = rc.Read(1)
	rc.Close()

	Dim unicode
	If Asc(test1) = 255 And Asc(test2) = 254 Then
		unicode = -1
	Else
		unicode = 0
	End If

	Set rc = fso.OpenTextFile(path_rc, 1, False, unicode)

	Dim path_tmp
	path_tmp = path_rc & ".tmp"
	Dim tmp
	Set tmp = fso.CreateTextFile(path_tmp, True, unicode)

	Dim count
	count = 0

	Dim updated
	updated = False

	Dim str
	Dim s
	Do While rc.AtEndOfStream = False
		str = rc.ReadLine()
		If count Mod 2 = 0 Then
			If re_begin.Test(str) Then
				count = count + 1
			End If
		Else

			If re_vernum.Test(str) Then
				Set s = re_vernum.Execute(str).Item(0).Submatches
				If s.Item(2) <> ver_num Then
					str = s.Item(0) & ver_num & s.Item(3)
					updated = True
				End If
			End If

			If re_verstr.Test(str) Then
				Set s = re_verstr.Execute(str).Item(0).Submatches
				If s.Item(2) <> ver_str Then
					str = s.Item(0) & ver_str & s.Item(3)
					updated = True
				End If
			End If

			If re_end.Test(str) Then
				count = count + 1
			End If

		End If
		tmp.WriteLine(str)
	Loop
	rc.Close()
	tmp.Close()

	If updated Then
		Call fso.DeleteFile(path_rc, 1)
		Call fso.MoveFile(path_tmp, path_rc)
		WScript.Echo(path_rc & ": updated version to """ & ver_str & """")
	Else
		Call fso.DeleteFile(path_tmp, 1)
		If count = 0 Then
			WScript.Echo(path_rc & ": no VS_VERSION_INFO found")
		End If
	End If
End Sub

'-----------------------------------------------------------------------------
Dim dir_base
dir_base = fso.GetParentFolderName(WScript.ScriptFullName) & "\"

Dim fld_base
Set fld_base = fso.GetFolder(dir_base)

Dim re_rc
Set re_rc = New RegExp
re_rc.Pattern = "\.rc$"
re_rc.IgnoreCase = True

Dim path_rc
For Each file In fld_base.Files
	If re_rc.Test(file.Name) Then
		Call UpdateVersion(dir_base & file.Name)
	End If
Next

