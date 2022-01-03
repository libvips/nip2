#ifndef __NIP_H
#define __NIP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define APP_ID "org.libvips.nip4"
#define APP_PATH "/org/libvips/nip4"

#include <gtk/gtk.h>

#include <vips/vips.h>

/* i18n placeholder.
 */
#define _(S) (S)

/* The tile size for image rendering.
 */
#define TILE_SIZE (256)

/* Cache size -- enough for two 4k displays. 
 */
#define MAX_TILES (2 * (4096 / TILE_SIZE) * (2048 / TILE_SIZE))

#include "gtkutil.h"
#include "app.h"
#include "main.h"

#endif /* __NIP_H */
