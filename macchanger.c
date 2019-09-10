#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <net/if_arp.h>
#include <errno.h>
#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <sys/socket.h>

extern int errno;

/*function prototype*/
static void get_current_mac();
static void get_permanent_mac();
static void change_mac(char *dev_name, char *new_mac );
static void mac_settings(GtkWidget *widget, gpointer ptr);

struct Application{
	/*main window*/
	GtkWidget *window;

	/*application settings*/
	GtkSettings *settings;

	/*icon*/
	GdkPixbuf *icon;

	/*main box*/
	GtkWidget *vbox;

	/*layout containers - horizontal boxes*/
	GtkWidget *device_hbox;
	GtkWidget *current_hbox;
	GtkWidget *permanent_hbox;
	GtkWidget *clone_hbox;
	GtkWidget *button_hbox;


	/*labels*/
	GtkWidget *device_label;
	GtkWidget *current_label;
	GtkWidget *permanent_label;
	GtkWidget *clone_label;

	/*entry*/
	GtkWidget *device_list;
	GtkWidget *current_entry;
	GtkWidget *permanent_entry;
	GtkWidget *clone_entry;

	/*buttons*/
	GtkWidget *button_mac;

	/*statusbar*/
	GtkWidget *statusbar;

};


struct Device{
	int sock;
	struct ifreq ifreq;
	char mac_buffer[49];
};

struct Device device;
struct Application application;

int main(int argv, char *argc[]){
	/*initialize gtk*/
	gtk_init( &argv, &argc);

	/*main window*/
	application.window = gtk_window_new( GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(application.window), "MacChanger");
	gtk_window_set_default_size(GTK_WINDOW(application.window), 400, 400);
	gtk_window_set_position(GTK_WINDOW(application.window), GTK_WIN_POS_MOUSE);
	gtk_container_set_border_width (GTK_CONTAINER(application.window), 3);
	gtk_window_set_resizable(application.window, FALSE );


	/*settings*/
	application.settings = gtk_settings_get_default();
	g_object_set(application.settings, "gtk-theme-name" , "Adwaita", NULL);

	/*icon*/
	GtkWidget *icon_image = gtk_image_new_from_file("icon.png");
	application.icon = gtk_image_get_pixbuf( icon_image );
	gtk_window_set_icon( application.window, application.icon );


	/*main layout*/
	application.vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
	gtk_box_set_homogeneous(application.vbox, TRUE);

	/*layout*/
	application.device_hbox= gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
	gtk_box_set_homogeneous(application.device_hbox, TRUE);

	application.current_hbox= gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
	gtk_box_set_homogeneous(application.current_hbox, TRUE);

	application.permanent_hbox= gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
	gtk_box_set_homogeneous(application.permanent_hbox, TRUE);

	application.clone_hbox= gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
	gtk_box_set_homogeneous(application.clone_hbox, TRUE);

	application.button_hbox= gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
	gtk_box_set_homogeneous(application.button_hbox, TRUE);

	/*labels*/
	application.device_label = gtk_label_new("select device");
	gtk_label_set_justify( application.device_label, GTK_JUSTIFY_LEFT);

	application.current_label = gtk_label_new("current mac address");
	gtk_label_set_justify( application.current_label, GTK_JUSTIFY_LEFT);

	application.permanent_label = gtk_label_new("permanent mac address");
	gtk_label_set_justify( application.permanent_label, GTK_JUSTIFY_LEFT);

	application.clone_label = gtk_label_new("clone mac address");
	gtk_label_set_justify( application.clone_label, GTK_JUSTIFY_LEFT);

	/*entry*/
	application.device_list = gtk_combo_box_text_new();

	struct if_nameindex *interfaces = if_nameindex();
	char *if_name = interfaces->if_name;

	while( (interfaces != NULL) && (if_name != '\0' )){
		gtk_combo_box_text_insert_text(application.device_list, interfaces->if_index, if_name);
		interfaces++;
		if_name = interfaces->if_name;
	}

	gtk_combo_box_set_active( application.device_list, 0);

	/*current mac address*/
	application.current_entry = gtk_entry_new();
	gtk_entry_set_text(application.current_entry, "");
	g_object_set(application.current_entry, "editable" , FALSE, NULL);
	gtk_entry_set_max_length(application.current_entry, 17);


	/*permanent mac address*/
	application.permanent_entry = gtk_entry_new();
	gtk_entry_set_text(application.permanent_entry, "");
	gtk_entry_set_max_length(application.permanent_entry, 17);
	g_object_set(application.permanent_entry, "editable" , FALSE, NULL);

	/*cloned mac address*/
	application.clone_entry = gtk_entry_new();
	gtk_entry_set_max_length(application.clone_entry, 17);
	g_object_set(application.current_entry, "editable" , TRUE, NULL);

	/*buttons*/
	application.button_mac = gtk_button_new_with_label("change mac");
	application.statusbar = gtk_statusbar_new();


	/*packing*/
	gtk_box_pack_start(application.device_hbox, application.device_label, FALSE, FALSE, 3);
	gtk_box_pack_start(application.device_hbox, application.device_list, FALSE, FALSE, 3);

	gtk_box_pack_start(application.current_hbox, application.current_label, FALSE, FALSE, 3);
	gtk_box_pack_start(application.current_hbox, application.current_entry, FALSE, FALSE, 3);

	gtk_box_pack_start(application.permanent_hbox, application.permanent_label, FALSE, FALSE, 3);
	gtk_box_pack_start(application.permanent_hbox, application.permanent_entry, FALSE, FALSE, 3);

	gtk_box_pack_start(application.clone_hbox, application.clone_label, FALSE, FALSE, 3);
	gtk_box_pack_start(application.clone_hbox, application.clone_entry, FALSE, FALSE, 3);

	gtk_box_pack_end(application.button_hbox, application.statusbar, TRUE, TRUE, 3);
	gtk_box_pack_end(application.button_hbox, application.button_mac, FALSE, FALSE, 3);

//	gtk_box_pack_start(application.permanent_hbox, application.permanent_entry, FALSE, FALSE, 3);


	gtk_box_pack_start(application.vbox, application.device_hbox, FALSE, FALSE, 3);
	gtk_box_pack_start(application.vbox, application.current_hbox, FALSE, FALSE, 3);
	gtk_box_pack_start(application.vbox, application.permanent_hbox, FALSE, FALSE, 3);
	gtk_box_pack_start(application.vbox, application.clone_hbox, FALSE, FALSE, 3);
	gtk_box_pack_start(application.vbox, application.button_hbox, FALSE, FALSE, 3);


	g_signal_connect(application.window, "delete_event", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(application.button_mac, "clicked", G_CALLBACK(mac_settings), NULL);

	/*add containers*/
	gtk_container_add(application.window, application.vbox);


	/*display all widgets*/
	gtk_widget_show_all(application.window);

	gtk_main();

	return 0;
}



static void mac_settings(GtkWidget *widget, gpointer ptr){
	get_current_mac(gtk_combo_box_text_get_active_text(application.device_list) );
	get_permanent_mac(gtk_combo_box_text_get_active_text(application.device_list) );

	if ( gtk_entry_get_text_length( application.clone_entry ) == 17 )
		change_mac(gtk_combo_box_text_get_active_text(application.device_list), "ee:13:12:be:2e:3f");

}



static void get_current_mac(char *dev_name){
	/*clear structure*/
	memset(&device, 0, sizeof(device));

	device.sock = socket(AF_INET, SOCK_DGRAM, 0);
	if( device.sock < 0){
		perror("error, cannot create socket\n");
		exit(EXIT_FAILURE);
	}

	strncpy( &device.ifreq.ifr_name, dev_name, IFNAMSIZ );

	if( ioctl( device.sock, SIOCGIFHWADDR, &device.ifreq) < 0 ){
		perror("error, cannot get mac address\n");
		exit( EXIT_FAILURE );
	}

	sprintf(device.mac_buffer, "%02x:%02x:%02x:%02x:%02x:%02x", (unsigned char)device.ifreq.ifr_hwaddr.sa_data[0], (unsigned char)device.ifreq.ifr_hwaddr.sa_data[1], (unsigned char)device.ifreq.ifr_hwaddr.sa_data[2], (unsigned char)device.ifreq.ifr_hwaddr.sa_data[3], (unsigned char)device.ifreq.ifr_hwaddr.sa_data[4], (unsigned char)device.ifreq.ifr_hwaddr.sa_data[5]);

	gtk_entry_set_text(application.current_entry, device.mac_buffer );

	close( device.sock );
}

static void change_mac( char *dev_name, char *new_mac ){
	if (strlen(new_mac) != 17 ){
		printf("error, incorect mac length\n");
		exit( EXIT_FAILURE);
	}

	/*clear structure*/
	memset(&device, 0, sizeof(device));

	device.sock = socket(AF_INET, SOCK_DGRAM, 0);
	if( device.sock < 0){
		perror("error, cannot create socket\n");
		exit(EXIT_FAILURE);
	}


	strncpy( &device.ifreq.ifr_name, dev_name, IFNAMSIZ );
	device.ifreq.ifr_hwaddr.sa_family = ARPHRD_ETHER;

	/*bring down interface*/
	device.ifreq.ifr_flags = IFF_UP;
	if (ioctl( device.sock ,SIOCSIFFLAGS, &device.ifreq ))
		perror("");

	/*FIXME: bring down interface*/



	for (int i = 0 ; i < 6 ; i++ ){
		device.ifreq.ifr_hwaddr.sa_data[i] = (unsigned char) (strtoul(new_mac+(i*3), 0, 16) & 0xFF);
	}


	if( ioctl( device.sock, SIOCSIFHWADDR, &device.ifreq) < 0 ){
		gtk_statusbar_push( application.statusbar, 0, strerror(errno) );		
	}else{
		gtk_entry_set_text(application.current_entry, new_mac );
		gtk_entry_set_text(application.clone_entry, new_mac );
		gtk_statusbar_push( application.statusbar, 0, "success");
	}

	close( device.sock );
}


static void get_permanent_mac(char *dev_name){
	/*clear structure*/
	memset(&device, 0, sizeof(device));

	device.sock = socket(AF_INET, SOCK_DGRAM, 0);
	if( device.sock < 0){
		perror("error, cannot create socket\n");
		exit(EXIT_FAILURE);
	}

	strncpy( &device.ifreq.ifr_name, dev_name, IFNAMSIZ );


	struct ethtool_perm_addr *epa;
	epa = (struct ethtool_perm_addr*) malloc(sizeof(struct ethtool_perm_addr) + IFHWADDRLEN);
	epa->cmd = ETHTOOL_GPERMADDR;
	epa->size = IFHWADDRLEN;

	device.ifreq.ifr_data = (caddr_t) epa;

	if (ioctl( device.sock, SIOCETHTOOL, &device.ifreq) < 0){
		perror("error, cannot get permanent mac address\n");
		exit( EXIT_FAILURE );
	}

	sprintf(device.mac_buffer, "%02x:%02x:%02x:%02x:%02x:%02x", (unsigned char)device.ifreq.ifr_hwaddr.sa_data[0], (unsigned char)device.ifreq.ifr_hwaddr.sa_data[1], (unsigned char)device.ifreq.ifr_hwaddr.sa_data[2], (unsigned char)device.ifreq.ifr_hwaddr.sa_data[3], (unsigned char)device.ifreq.ifr_hwaddr.sa_data[4], (unsigned char)device.ifreq.ifr_hwaddr.sa_data[5]);

	gtk_entry_set_text(application.permanent_entry, device.mac_buffer );


	
	free(epa);
	close( device.sock );
}


