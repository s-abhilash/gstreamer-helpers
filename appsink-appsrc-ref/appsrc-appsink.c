/* GStreamer
 *
 * appsink-src.c: example for using appsink and appsrc.
 *
 * Copyright (C) 2008 Wim Taymans <wim.taymans@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <gst/gst.h>

#include <string.h>
#include <stdio.h>

#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>

/* these are the caps we are going to pass through the appsink and appsrc */
const gchar *audio_caps =
"audio/x-raw,format=S16LE,channels=1,rate=8000, layout=interleaved";

typedef struct
{
	GMainLoop *loop;
	GstElement *source;
	GstElement *sink;
} ProgramData;


/* Gstreamer free buffer callback */
static void sFW_FreePtr(gpointer apData)
{
		if (apData)
		{
			printf("sFW_FreePtr\n");
			free(apData);
		}
}

/* called when the appsink notifies us that there is a new buffer ready for
 * processing */
	static GstFlowReturn
on_new_sample_from_sink (GstElement * elt, ProgramData * data)
{
	GstSample *sample;
	GstBuffer *app_buffer, *buffer;
	GstElement *source;
	GstFlowReturn ret;
  	GstMapInfo imap;
	char *sdata;

	/* get the sample from appsink */
	sample = gst_app_sink_pull_sample (GST_APP_SINK (elt));
	buffer = gst_sample_get_buffer (sample);
    gst_buffer_map (buffer, &imap, GST_MAP_READ);
	sdata = imap.data;
	printf("buffer val -> %d\n", sdata[0]);

	/* we don't need the appsink sample anymore */
	gst_sample_unref (sample);
	fprintf(stderr, "New sample\n");

	return GST_FLOW_OK;
}

/* called when we get a GstMessage from the source pipeline when we get EOS, we
* notify the appsrc of it. */
static gboolean
on_source_message (GstBus * bus, GstMessage * message, ProgramData * data)
{
   GstElement *source;
 
   switch (GST_MESSAGE_TYPE (message)) {
     case GST_MESSAGE_EOS:
       g_print ("The source got dry\n");
       g_main_loop_quit (data->loop);
       break;
     case GST_MESSAGE_ERROR:
       g_print ("Received error\n");
       g_main_loop_quit (data->loop);
       break;
     default:
       break;
   }
   return TRUE;
}

int
main (int argc, char *argv[])
{
  gchar *filename = NULL;
  ProgramData *data = NULL;
  gchar *string = NULL;
  GstBus *bus = NULL;
  GstElement *testsink = NULL;
  GstElement *testsource = NULL;
  GstBuffer *pBuffer;

  gst_init (&argc, &argv);

  if (argc == 2)
    filename = g_strdup (argv[1]);
  else
    filename = g_strdup ("/usr/share/sounds/ekiga/ring.wav");

  if (!g_file_test (filename, G_FILE_TEST_EXISTS)) {
    g_print ("File %s does not exist\n", filename);
    g_free (filename);
    return -1;
  }

  data = g_new0 (ProgramData, 1);

  data->loop = g_main_loop_new (NULL, FALSE);

  /* setting up source pipeline, we read from a file and convert to our desired
   * caps. */
  string =
      g_strdup_printf
      ("appsrc name=testsource ! truehddec ! appsink name=testsink");
  g_free (filename);
  data->source = gst_parse_launch (string, NULL);
  g_free (string);

  if (data->source == NULL) {
    g_print ("Bad source\n");
    g_main_loop_unref (data->loop);
    g_free (data);
    return -1;
  }

  /* to be notified of messages from this pipeline, mostly EOS */
  bus = gst_element_get_bus (data->source);
  gst_bus_add_watch (bus, (GstBusFunc) on_source_message, data);
  gst_object_unref (bus);

  /* we use appsink in push mode, it sends us a signal when data is available
   * and we pull out the data in the signal callback. We want the appsink to
   * push as fast as it can, hence the sync=false */
  testsink = gst_bin_get_by_name (GST_BIN (data->source), "testsink");
  g_object_set (G_OBJECT (testsink), "emit-signals", TRUE, "sync", FALSE, NULL);
  g_signal_connect (testsink, "new-sample",
      G_CALLBACK (on_new_sample_from_sink), data);
  gst_object_unref (testsink);

  testsource = gst_bin_get_by_name (GST_BIN (data->source), "testsource");


  gst_element_set_state (data->source, GST_STATE_PLAYING);


  for (int i = 0; i < 10; i++)
  {
  	 char *sData = malloc(2048);
	 sData[0] = i;
	  pBuffer = gst_buffer_new_wrapped_full( 0, (gpointer)(sData), 2048, 0, 2048, (gpointer)sData, sFW_FreePtr);
	  GstFlowReturn nRet = gst_app_src_push_buffer((GstAppSrc *)testsource, pBuffer);

	  if(nRet !=  GST_FLOW_OK)
		  printf("push buffer returned !GST_FLOW_OK\n");
	  else
		  printf("push buffer returned GST_FLOW_OK\n");
  }
  gst_app_src_end_of_stream((GstAppSrc*)testsource);

  /* let's run !, this loop will quit when the sink pipeline goes EOS or when an
   * error occurs in the source or sink pipelines. */
  g_print ("Let's run!\n");
  g_main_loop_run (data->loop);
  g_print ("Going out\n");

  gst_element_set_state (data->source, GST_STATE_NULL);

  gst_object_unref (data->source);
  g_main_loop_unref (data->loop);
  g_free (data);

  return 0;
}
