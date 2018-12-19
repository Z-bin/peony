#include "mime-utils.h"
#include "office-utils.h"
#include <gdk/gdkx.h>
#include <glib/gstdio.h>

#include <libpeony-private/peony-signaller.h>

unoconv_child_watch_cb2 (GPid pid,
                        gint status,
                        gpointer user_data)
{
	//printf("unoconv_child_watch_cb pid: %d\n",pid);
	g_spawn_close_pid (pid);
	g_signal_emit_by_name (PEONY_WINDOW_INFO (user_data), "office_trans_ready", NULL);    //office ready cb , if pdf name = global preview file name, show it. else g_remove file.
}

char* office2pdf_by_window (PeonyWindowInfo *window, char *filename){

    if (old_pid != -1){
        gchar *cmd = g_strdup_printf ("kill %d\n", old_pid);
        printf("cmd :%s", cmd);
        //system (cmd);
        kill (old_pid, SIGKILL); //kill old pid for office transform anyway
        old_pid = -1;
        g_free (cmd);
	}

	gchar *doc_path, *pdf_path, *tmp_name, *tmp_path, *quoted_path;
	GFile *file;
	gboolean res;
	gchar *cmd;

	gint argc;
	GPid pid;
	gchar **argv = NULL;
	GError *error = NULL;
	const gchar *unoconv_path;

	unoconv_path = g_find_program_in_path ("unoconv");
	if (unoconv_path == NULL) {
		printf("unoconv is not found\n");
		//openoffice_missing_unoconv (self);
		return NULL;
	}

	//printf("pid: %d\n",getpid());

	file = g_file_new_for_path(filename);
	tmp_name = g_strdup_printf("%s.pdf",g_file_get_basename (file));
	
	//printf("tmp_name: %s\n",tmp_name);
	tmp_path = g_build_filename (g_get_user_cache_dir (), "peony", NULL);
	pdf_path = g_build_filename (tmp_path, tmp_name, NULL);
	g_mkdir_with_parents (tmp_path, 0700);

	cmd = g_strdup_printf ("unoconv -f pdf -o %s '%s'", pdf_path, filename);
	//printf("cmd: %s\n",cmd);
	
	res = g_shell_parse_argv (cmd, &argc, &argv, &error);
	g_free (cmd);

	if (!res) {
		g_warning ("Error while parsing the unoconv command line: %s",
				error->message);
		g_error_free (error);

		return NULL;
	}

	res = g_spawn_async (NULL, argv, NULL,
			G_SPAWN_DO_NOT_REAP_CHILD |
			G_SPAWN_SEARCH_PATH,
			NULL, NULL,
			&pid, &error);

	g_strfreev (argv);

	if (!res) {
		g_warning ("Error while spawning unoconv: %s",
				error->message);
		g_error_free (error);

		return NULL;
	}

	g_child_watch_add (pid, unoconv_child_watch_cb2, window);

	//g_spawn_close_pid(pid);
	old_pid = pid;

	return pdf_path; //it will be saved in global preview filename, and wait for office ready cb if selection not changed.

}

char* excel2html_by_window (PeonyWindowInfo *window, char *filename){

    if (old_pid != -1){
        gchar *cmd = g_strdup_printf ("kill %d", old_pid);
        printf("cmd :%s\n", cmd);
        //system (cmd);
        kill (old_pid, SIGKILL); //kill old pid for office transform anyway
        old_pid = -1;
        g_free (cmd);
	}

	gchar *doc_path, *html_path, *tmp_name, *tmp_path, *quoted_path;
	GFile *file;
	gboolean res;
	gchar *cmd;

	gint argc;
	GPid pid;
	gchar **argv = NULL;
	GError *error = NULL;
	const gchar *unoconv_path;

	unoconv_path = g_find_program_in_path ("unoconv");
	if (unoconv_path == NULL) {
		printf("unoconv is not found\n");
		//openoffice_missing_unoconv (self);
		return NULL;
	}

	//printf("pid: %d\n",getpid());

	file = g_file_new_for_path(filename);
	tmp_name = g_strdup_printf("%s.html",g_file_get_basename (file));

	//printf("tmp_name: %s\n",tmp_name);
	tmp_path = g_build_filename (g_get_user_cache_dir (), "peony", NULL);
	html_path = g_build_filename (tmp_path, tmp_name, NULL);
	g_mkdir_with_parents (tmp_path, 0700);

	cmd = g_strdup_printf ("unoconv -f html -o %s '%s'", html_path, filename);
	//printf("cmd: %s\n",cmd);	
	
	res = g_shell_parse_argv (cmd, &argc, &argv, &error);
	g_free (cmd);

	if (!res) {
		g_warning ("Error while parsing the unoconv command line: %s",
				error->message);
		g_error_free (error);

		return NULL;
	}

	res = g_spawn_async (NULL, argv, NULL,
			G_SPAWN_DO_NOT_REAP_CHILD |
			G_SPAWN_SEARCH_PATH,
			NULL, NULL,
			&pid, &error);

	g_strfreev (argv);

	if (!res) {
		g_warning ("Error while spawning unoconv: %s",
				error->message);
		g_error_free (error);

		return NULL;
	}

	g_child_watch_add (pid, unoconv_child_watch_cb2, window);

	old_pid = pid;

	return html_path; //it will be saved in global preview filename, and wait for office ready cb if selection not changed.

}