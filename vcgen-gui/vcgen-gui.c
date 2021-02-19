#include <gtk/gtk.h>
#include <libgen.h>
#include <string.h>
#include <math.h>

GtkWidget* window;
GtkBuilder* builder; 

GtkEntry* entry_vcexepath;
GtkEntry* entry_vcgenpath;

GtkEntry* entry_targetdir;

GtkEntry* entry_passpref;
GtkEntry* entry_passpost;

GtkComboBoxText* combo_fs;
GtkComboBoxText* combo_cs;

GtkSpinButton* spin_passlen;
GtkSpinButton* spin_csize;
GtkSpinButton* spin_cnum;

GtkCheckButton* check_cnum;

const char *fstypes[] = {"FAT", "NTFS", "ext4"};
int cnum_inf = 0;

void loadWidgets()
{
    entry_vcexepath = (GtkEntry*)(gtk_builder_get_object(builder, "entry_vcexepath"));
    entry_vcgenpath = (GtkEntry*)(gtk_builder_get_object(builder, "entry_vcgenpath"));
    
    entry_targetdir = (GtkEntry*)(gtk_builder_get_object(builder, "entry_targetdir"));

    entry_passpref = (GtkEntry*)(gtk_builder_get_object(builder, "entry_passprefix"));
    entry_passpost = (GtkEntry*)(gtk_builder_get_object(builder, "entry_passpostfix"));
    
    combo_fs = (GtkComboBoxText*)(gtk_builder_get_object(builder, "combo_fs"));
    combo_cs = (GtkComboBoxText*)(gtk_builder_get_object(builder, "combo_cs"));
    
    spin_passlen = (GtkSpinButton*)(gtk_builder_get_object(builder, "spin_passlen"));
    spin_csize = (GtkSpinButton*)(gtk_builder_get_object(builder, "spin_csize"));
    spin_cnum = (GtkSpinButton*)(gtk_builder_get_object(builder, "spin_cnum"));
    
    check_cnum = (GtkCheckButton*)(gtk_builder_get_object(builder, "check_cnum"));
}

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    builder = gtk_builder_new_from_file("vcgen_gui_window.glade");

    window = GTK_WIDGET(gtk_builder_get_object(builder, "vcgen_gui_window"));
    gtk_builder_connect_signals(builder, NULL);
    loadWidgets();

    g_object_unref(builder);

    gtk_widget_show(window);
    gtk_main();

    return 0;
}

char* openFileDialog()
{
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;

    GtkWidget* dialog = gtk_file_chooser_dialog_new("Open File", NULL, action, "_Cancel", GTK_RESPONSE_CANCEL, "_Open", GTK_RESPONSE_ACCEPT, NULL);

    gint res = gtk_dialog_run (GTK_DIALOG (dialog));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        char* filename;
        GtkFileChooser* chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);
        gtk_widget_destroy(dialog);
        return filename;
    }

    gtk_widget_destroy (dialog);
    return NULL;
}

char* selectFolderDialog()
{
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;

    GtkWidget* dialog = gtk_file_chooser_dialog_new("Select folder", NULL, action, "_Cancel", GTK_RESPONSE_CANCEL, "_Open", GTK_RESPONSE_ACCEPT, NULL);

    gint res = gtk_dialog_run (GTK_DIALOG (dialog));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        char* filename;
        GtkFileChooser* chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);
        gtk_widget_destroy(dialog);
        return filename;
    }

    gtk_widget_destroy (dialog);
    return NULL;
}

void on_btn_vcexepath_clicked()
{
    char* path = openFileDialog();
    if(path != NULL)
        gtk_entry_set_text(entry_vcexepath, path);
    
    g_free(path);
}

void on_btn_vcgenpath_clicked()
{
    char* path = openFileDialog();
    if(path != NULL)
        gtk_entry_set_text(entry_vcgenpath, path);
    
    g_free(path);
    
}

void on_btn_targetpath_clicked()
{
    char* path = selectFolderDialog();
    if(path != NULL)
        gtk_entry_set_text(entry_targetdir, path);
    
    g_free(path);
}

void on_btn_start_clicked()
{
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    
    int combo_fs_i = gtk_combo_box_get_active((GtkComboBox*)combo_fs);
    int combo_cs_i = gtk_combo_box_get_active((GtkComboBox*)combo_cs);
    int spin_passlen_val = gtk_spin_button_get_value(spin_passlen);
    int spin_csize_val = gtk_spin_button_get_value(spin_csize);
    int spin_cnum_val = gtk_spin_button_get_value(spin_cnum);

    const char* vcexe_path = gtk_entry_get_text(entry_vcexepath);
    const char* vcgen_path = gtk_entry_get_text(entry_vcgenpath);
    
    const char* container_path = gtk_entry_get_text(entry_targetdir);

    const char* pass_pref = gtk_entry_get_text(entry_passpref);
    const char* pass_post = gtk_entry_get_text(entry_passpost);

    if(strlen(pass_pref) + strlen(pass_post) > spin_passlen_val)
	return;

    int cnum_val = spin_cnum_val;
    if(cnum_inf)
        cnum_val = -1;

    /*
    printf("vcexe_path: %s\n", vcexe_path);
    printf("vcgen_path: %s\n", vcgen_path);
    printf("container_path: %s\n", container_path);
    printf("COMBO_FS_I: %d\n", combo_fs_i);
    printf("COMBO_CS_I: %d\n", combo_cs_i);
    printf("spin_passlen_val: %d\n", spin_passlen_val);
    printf("spin_csize_val: %d\n", spin_csize_val);
    printf("spin_cnum_val: %d\n", spin_cnum_val);
    printf("cnum_inf: %d\n", cnum_inf);
    */

    unsigned long container_size = spin_csize_val * pow(1000, combo_cs_i);
    
    char buffer[255];
    snprintf(buffer, sizeof(buffer), "%s %s %d %d %lu %s --vcexec=\"%s\" --ppref=\"%s\" --ppost=\"%s\"", vcgen_path, container_path, spin_passlen_val, cnum_val, container_size, fstypes[combo_fs_i], vcexe_path, pass_pref, pass_post);

    //printf("buffer: %s\n", buffer);

    system(buffer);
}

void on_check_cnum_toggled()
{
    cnum_inf = !cnum_inf;
}

void on_vcgen_gui_window_destroy()
{
    printf("\nExit\n");
    gtk_main_quit();
}
