ENTRY(StackAlign)
SECTIONS
{
	.text :
	{
		*(.text.StackAlign)
		*(.text)
	}
}
