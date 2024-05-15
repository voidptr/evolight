from OPCConnection import OPCConnection
from CursesController import CursesController

class InterfaceController:
	def __init__(self,
		## Open Pixel Server Configuration
		opc_server="127.0.0.1",
		opc_port="7890",
		use_opc=True,
		interpol=False,

		## Curses Display
		use_curses=False,

		## Call-backs
		reset_ga_hook=None,
		get_colors_hook=None,
		get_status_hook=None,
		quit_hook=None,

		debug_handle=None,

		## piddly things
		verbose=False,
		debug=False
	):

		self.server = opc_server
		self.port = opc_port
		self.use_opc = use_opc
		self.interpol = interpol

		self.use_curses = use_curses

		self.reset_ga_hook = reset_ga_hook
		self.get_colors_hook = get_colors_hook
		self.get_status_hook = get_status_hook
		self.quit_hook = quit_hook

		self.debug_handle = debug_handle

		self.verbose = verbose
		self.debug = debug

		self.initted = False

	def init(self):
		## init the OPC connection
  		if self.use_opc:
  			self.conn = OPCConnection(self.server, self.port)
			self.conn.init()

		## set up the curses interface
		if self.use_curses:
			self.cwin = CursesController(debug_handle=self.debug_handle)
			self.cwin.init()
			self.cwin.set_title("EVOLIGHT","hihihihi")
			self.cwin.prep_for_input()
		self.initted = True

	def cleanup(self):
		if self.use_curses:
			self.cwin.cleanup()

	def input_process(self):
		if self.use_curses:
			self.handle_keypress()

	def handle_keypress(self):
	    keypress = self.cwin.get_keypress()

	    if keypress == ord('q'):
	    	self.cwin.clear_input()
	    	self.quit_hook()

	    elif keypress == ord('v'):
	    	self.cwin.clear_input()
	        self.verbose = not self.verbose

	    elif keypress == ord('r'):
	    	self.cwin.clear_input()
	        self.reset_ga_hook()

	def output_process(self):
		if self.use_opc:
			if self.interpol:
				self.conn.display(self.get_colors_hook())
			else:
				self.conn.interpolated(self.get_colors_hook())

		if self.use_curses:
			self.cwin.display(self.get_colors_hook())
			self.cwin.refresh_display()

		if self.verbose:
			if self.use_curses:
				self.cwin.status(self.get_status_hook())
			else:
				print self.get_status_hook()



