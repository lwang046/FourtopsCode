import logging

from ROOT import TFile
from ROOT import TCanvas
from ROOT import TPad
from ROOT import TH1
from ROOT import TObject
from ROOT import THStack
from ROOT import TLegend
from ROOT import TAxis

TH1.__init__._creates = False
THStack.__init__._creates = False
TPad.__init__._creates = False
TCanvas.__init__._creates = False
TLegend.__init__._creates = False

from pprint import pprint,pformat

from progress_bar import progress


class plotter:
	"""
	Binned comparison plotting tool
	"""	

	def __init__(self, **kwargs):
		self.out_format=kwargs.get('format_plot',".png")
		self.source_root = None
		self.reference_root = None

	def set_file(self,root_file):
		input_file = TFile.Open(root_file,'READ')
		if not input_file:
			raise NameError('Cannot read ROOT file: %s. Terminating!' % root_file)
		self.source_root = input_file

	def set_reference_file(self,root_file):
		reference_file = TFile.Open(root_file,'READ')
		if not reference_file:
			raise NameError('Cannot read ROOT file: %s. Terminating!' % root_file)
		self.reference_root = reference_file

	def decorate_page(self,master_canvas,config):
		"""
		decorate_page
		"""

		# Set axis label size
		for pad_id, c in enumerate(config['pads']):
			gPad = master_canvas.cd(1).cd(pad_id+1)
			pad_objs = gPad.GetListOfPrimitives()
			pad_num = gPad.GetNumber() - 1
			col = pad_num % config["nx"]
			row = (pad_num - col) / config["nx"]
			st = pad_objs.FindObject("st")
			if st:
				st.GetYaxis().SetLabelSize(0.)
				st.GetXaxis().SetLabelSize(0.)
			if col == 0 and st:
				st.GetYaxis().SetLabelSize(0.2)
			if row == config["ny"]-1 and st: 
				st.GetXaxis().SetLabelSize(0.2)

		# Add legend
		tmp = master_canvas.cd(1).cd(1).GetListOfPrimitives()
		leg1 = TLegend(config["legend"]["coord"][0],config["legend"]["coord"][1],config["legend"]["coord"][2],config["legend"]["coord"][3])
		leg1.SetBorderSize(0)
		leg1.SetTextSize(0.1);
		for entry in tmp:
			if entry.IsA().InheritsFrom("THStack"):	
				for hist in entry.GetHists():
					leg1.AddEntry(hist,"","l")
			else: pass
		
		master_canvas.cd(2)
		leg1.Draw()
		return

	def decorate_pad(self, pad, stack, gPad):
		"""
		decorate_pad
		"""
		stack.GetXaxis().SetNdivisions(5,0,0)
		stack.GetYaxis().SetNdivisions(5,0,0)
		if "title" in pad: stack.SetTitle(pad["title"])
		if "ymax" in pad: stack.SetMaximum(pad["ymax"])
	 	if "ymin" in pad: stack.SetMinimum(pad["ymin"])
		return

	def make_pad(self, pad_tup, root_canvas):
		"""
		make_pad
		"""
		logging.debug(pformat(pad_tup[1]))
	      	gPad = root_canvas.cd(pad_tup[0]+1)
		gPad.SetFillStyle(0)	#transparent pad
		st = None
		if 'refhistogram' in pad_tup[1]:
			st = self.make_stackplot_ratio(pad_tup[1],gPad)
		else:
			st = self.make_stackplot(pad_tup[1],gPad)
	      	self.decorate_pad(pad_tup[1], st, gPad)
	      	return

	def make_pads(self,pads,root_canvas):
		"""
		make_pads
		"""
		for pad in enumerate(pads):
			self.make_pad(pad,root_canvas)
		return

	def make_pages(self,pages_config):
		"""
		make_page
		"""
		canvas = pages_config['canvas']
		i, total = (0, len(canvas))
		for c in canvas:
			##############################
			progress(i, total, 'Progress')
			i += 1
			#############################
			root_canvas = TCanvas('c','CMS',5,45,c['width'],c['height'])
			root_canvas.Divide(2,1); root_canvas.cd(1).SetPad(0.1,0.1,0.7,0.9); root_canvas.cd(2).SetPad(0.7,0.1,0.9,0.9)
			root_canvas.cd(1).Divide(c['nx'],c['ny'],0.,0.)
			self.make_pads(c['pads'],root_canvas.cd(1))
	  		self.decorate_page(root_canvas,c)
			if 'name' in c: root_canvas.Print(c['name'])
			else: root_canvas.Print(str('file_'+str(i)+self.out_format))
	  	return

	def make_stackplot(self, pad, gPad):
		"""
		make_stackplot
		"""

		st = THStack("st","TEST")
		for hist in pad["histograms"]:
			logging.debug("ROOT FILE: %s" % self.source_root)
			logging.debug("NAME HIST: %s" % hist)
			h = None

			if "name" not in hist: continue

			if self.source_root: h = self.source_root.Get(str(hist["name"])).Clone()
			else: raise NameError('Wrong source ROOT file handle: %s. Terminating!' % self.source_root)
			h = self.source_root.Get(str(hist["name"])).Clone()
			if not h:
				raise NameError('Cannot read ROOT histogram: %s. Terminating!' % str(hist["name"]))	

			# Set style
			if "style" in hist:  self.set_hist_style(h,hist)
			######################
			st.Add(h,'hist')

		gPad.cd()
		st.Draw("nostack")
		return st

	def make_stackplot_ratio(self, pad, gPad):
		"""
		make_ratio_stackplot
		"""

		logging.debug(pformat(pad))

		st = THStack("st","TEST")
		for hist in pad["histograms"]:
			logging.debug("ROOT FILE: %s" % self.source_root)
                        logging.debug("NAME HIST: %s" % hist)
			h = None

			if "name" not in hist: continue

			if self.source_root: h = self.source_root.Get(str(hist["name"])).Clone()
			else: raise NameError('Wrong source ROOT file handle: %s. Terminating!' % self.source_root)
			h = self.source_root.Get(str(hist["name"])).Clone()
			if not h:
				raise NameError('Cannot read ROOT histogram: %s. Terminating!' % str(hist["name"]))	
			href = None
			if self.reference_root: href = self.reference_root.Get(str(pad["refhistogram"])).Clone()
			else: raise NameError('Wrong reference ROOT file handle: %s. Terminating!' % self.reference_root)
			if not href:
				raise NameError('Cannot read ROOT histogram: %s. Terminating!' % str(pad["refhistogram"]))	

			# Set style
			if "style" in hist:  self.set_hist_style(h,hist)
			######################
			h.Add(href,-1.)
			h.Divide(href)
			st.Add(h,'hist')

		gPad.cd()
		st.Draw("nostack")
		return st

	def set_hist_style(self, hist, style):
		""" Set style """
		if not hist:
			raise NameError('Cannot read ROOT histogram. Terminating!')	
		if "style" in style:
			hist.SetLineWidth(style["style"][0])
			hist.SetLineColor(style["style"][1])
			hist.SetLineStyle(style["style"][2])
		if "title" in style:
			hist.SetTitle(style["title"])
		return

