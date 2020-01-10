ENTRY(_WindowsEntrypoint)
SECTIONS
{
	text : 
	{
		*(.text.WindowsEntrypoint)
		*(.text)
	}
}
