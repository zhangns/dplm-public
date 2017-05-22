function [opt unprocessed_arg] = getopt(opt,varargin)
% Process a variable number of options like getopt in C
% Originally posted by AJ Johnson, modified by Chi-Hang Lam, Jan 2014
%
% Usage: 
%   opt = getopt( struct( '<opt>',<default>, '<opt>',<default>, ... ), varargin{:} )
%   [opt vararg] = getopt( struct( '<opt>',<default>, '<opt>',<default>, ... ), varargin{:} )
%
% Option with a single argument: 
%   For each option '<opt>' found in varargin, the next argument in varargin is assigned to opt.<opt>. 
%   Otherwise, <default> is assigned to opt.<opt>. 
% Switch:
%   If <default> equals 'noarg', opt.<opt> is assigned 1 if '<opt>' is found in varargin. Otherwise it equals 0. 
% vararg returns a list of unmatched entries in varargin, useful as arguments for a further function call.
% Options in varargin can appear in arbitrary order.
%  
% EXAMPLE:
% 
% varargin = {'length',10, 'cube', 'density',13}; % typically assigned via a function call instead
% opt = getopt(struct('length',0, 'width',0, 'height',0, 'density',1, 'cube','noarg' ), varargin{:});
% would return:
% opt = 
%     length: 10
%      width: 0
%     height: 0
%    density: 13
%       cube: 1
  
outflg = (nargout == 2); % to output unprocessed arg
prop_names = fieldnames(opt);
TargetField = [];
arglist = [];
for ii=1:length(varargin)
  arg = varargin{ii};
  if isempty(TargetField)
    % get field name
    f = find(strcmp(prop_names, arg));
    if ~ischar(arg) | length(f) == 0
      if outflg
        continue; % neglect unknown field
      else
        prop_names
        error('%s ',['invalid property ''',arg,'''; must be one of:'], prop_names{:}); % quit
      end
    end
    TargetField = arg;
    if (ischar(opt.(TargetField))) & strcmp(opt.(TargetField),'noarg')
      opt.(TargetField) = 1;
      TargetField = '';
    end;
  else
    % get value
    opt.(TargetField) = arg;
    TargetField = '';
  end
  arglist = [arglist ii];
end

if ~isempty(TargetField)
  error('Property names and values must be specified in pairs.');
end
for jj=1:length(prop_names) % replace all noarg by 0 for all off-flags
  if ischar(opt.(prop_names{jj})) & strcmp(opt.(prop_names{jj}),'noarg')
    opt.(prop_names{jj}) = 0;
  end;
end;
if outflg
  varargin(arglist) = []; % delete processed arguments
  unprocessed_arg =  varargin;
end;

