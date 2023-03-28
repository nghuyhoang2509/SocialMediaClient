static void add_post_to_list(const char *mail, const char *content);

static GtkListBox *list_post;

static void add_post_to_list(const char *mail, const char *content)
{
    GtkWidget *label_mail = gtk_label_new(mail);
    GtkWidget *label_content = gtk_label_new(content);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    GtkWidget *row = gtk_list_box_row_new();
    GtkStyleContext *context = gtk_widget_get_style_context(row);
    GtkStyleContext *context_label_mail = gtk_widget_get_style_context(label_mail);
    GtkStyleContext *context_label_content = gtk_widget_get_style_context(label_content);
    gtk_style_context_add_class(context, "post");
    gtk_style_context_add_class(context_label_mail, "h2");
    gtk_style_context_add_class(context_label_content, "body");

    gtk_label_set_line_wrap(GTK_LABEL(label_content), TRUE);
    gtk_label_set_line_wrap_mode(GTK_LABEL(label_content), PANGO_WRAP_WORD);
    gtk_box_pack_start(GTK_BOX(box), label_mail, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), label_content, FALSE, FALSE, 0);

    gtk_widget_set_halign(row, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(row, GTK_ALIGN_CENTER);
    gtk_container_add(GTK_CONTAINER(row), box);
    gtk_list_box_insert(list_post, row, -1);
}

int Discover()
{
    char data[2];
    sprintf(data, "{}");
    printf("Loading discover, Please Wait");
    char *response = request("all-post", data);
    json_object *root = json_tokener_parse(response);
    json_object *post_obj = json_object_object_get(root, "posts");
    int post_array_len = json_object_array_length(post_obj);

    GtkBuilder *builder = gtk_builder_new_from_file("./pages/Discover/Discover.glade");

    GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "discover_page"));

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "./pages/Discover/style.css", NULL);

    GtkWidget *label_name = GTK_WIDGET(gtk_builder_get_object(builder, "label_name"));
    GtkWidget *list_box = GTK_WIDGET(gtk_builder_get_object(builder, "list_box"));

    list_post = GTK_LIST_BOX(list_box);
    for (int i = 0; i < post_array_len; i++)
    {
        json_object *post_string = json_object_array_get_idx(post_obj, i);
        json_object *post_json = json_tokener_parse(json_object_get_string(post_string));
        json_object *mail_post;
        json_object *content_post;
        json_object_object_get_ex(post_json, "mail", &mail_post);
        json_object_object_get_ex(post_json, "content", &content_post);

        add_post_to_list(json_object_get_string(mail_post), json_object_get_string(content_post));
        json_object_put(post_string);
    }
    json_object_put(root);

    css_set(window, provider);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    g_signal_connect(window, "destroy", G_CALLBACK(come_back), NULL);
    gtk_window_maximize(GTK_WINDOW(window));
    gtk_widget_show_all(window);

    gtk_main();

    return EXIT_SUCCESS;
}