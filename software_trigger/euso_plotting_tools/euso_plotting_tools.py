import numpy as np
import matplotlib.pyplot as plt
import matplotlib as mpl
import matplotlib.cm as cm 

# Make function to plot
def plot_focal_surface(focal_surface):
    fig = plt.figure()
    ax = fig.add_subplot(111)
    p=plt.imshow(focal_surface, axes=ax, cmap=cm.cubehelix, interpolation='nearest')
    plt.xlabel('pixel X')
    plt.ylabel('pixel Y')

    # Set ticks
    major_ticks = np.arange(-.5, 48, 8)
    minor_ticks = np.arange(-.5, 48, 1)
    #ax.set_xlim(0.0,47.0)
    ax.set_xticks(major_ticks)
    ax.set_xticks(minor_ticks, minor=True)
    ax.set_yticks(major_ticks)
    ax.set_yticks(minor_ticks, minor=True)
    ax.set_xticklabels(np.arange(0, 49, 8));
    ax.set_yticklabels(np.arange(48, 0, -8));

    # Set grid
    #ax.grid(which='minor', alpha=0.2)
    ax.grid(color='k', linestyle='-', linewidth=2)
    ax.grid(which='major', alpha=0.4)

    # Add colourbar
    fig.subplots_adjust(right=0.8)
    cbar_ax = fig.add_axes([0.8, 0.1, 0.05, 0.8])
    cbar=fig.colorbar(p, cax=cbar_ax)
    cbar.set_label('# of photons', labelpad=1)
    cbar.formatter.set_powerlimits((0, 0))


# Make function to animate packets around trigger
# Not yet working...
from matplotlib import animation, rc
from IPython.display import HTML
rc('animation', html='html5')
def anim_focal_surface():
    fig = plt.figure()
    nt=0
    ims = []
    for add in np.arange(10):
        im = plt.imshow(np.array(counts[int(frame[nt] + add)][:]).reshape(48, 48),
                              cmap = cm.cubehelix, interpolation = 'nearest', animated = True)
        ims.append([im])

    im_ani = animation.ArtistAnimation(fig, ims, interval=50, repeat_delay=3000, blit = False)
    HTML(im_ani.to_html5_video())
                                  
